# Protocol core

The ARINC 615A protocol core — `lib/arinc_615a` — is shared, unmodified, with the
command-line tool suite. It has no Qt dependency and no knowledge that a GUI
exists.

This document covers the layers **below** the operation adapters: the shared
operation state machine, the protocol file codec, what ARINC 615A adds to TFTP,
the timer and abort model, and the status code table. For everything **above**
them — the wizard shell, the adapters, the thread boundary — see
[CODE-TRACE.md](CODE-TRACE.md), and for the shape of the whole thing see
[ARCHITECTURE.md](ARCHITECTURE.md).

Nothing here is GUI-specific. It is reproduced in this repository rather than
linked so that the repository stands on its own.

> **Where the GUI sits.** Each of the five classes in
> `lib/arinc_615a_dla_qt/operations/` implements one of this layer's handler
> interfaces and re-emits every callback as a Qt signal. The handler methods
> described below therefore run on the ASIO I/O thread, never the GUI thread.

---

## Contents

| § | Subject |
| --- | --- |
| [1](#1--the-shared-operation-state-machine) | `OperationImpl` — initialisation handshake, DLP watchdog, abort delivery |
| [2](#2--protocol-file-codec) | Header, strings, ratios, the filename table, file bodies |
| [3](#3--what-arinc-615a-adds-to-tftp) | Option names, WAIT and ABORT error packets, the DLP retry layer |
| [4](#4--timers-and-abort) | Three nested timers, abort versus terminate |
| [5](#5--status-codes) | The full status code table |

---

## 1 — The shared operation state machine

Every TFTP-based operation derives from `Arinc615a::Host::OperationImpl`. It owns the TFTP client, the TFTP server, the DLP timeout timer, the abort state, and the protocol file logger — and it implements the initialisation handshake that all four operations begin with. The derived classes supply only `start()`, `tftpRequest()`, and their own file handling.

The asymmetry to hold onto: the host is a TFTP **client** when it reads the initialisation file and writes request files, and a TFTP **server** for everything the target pushes back — status files, list files, and data files. Both run simultaneously. Initialisation is a gate: the operation body is only entered on `OperationAccepted`. Once inside, the DLP watchdog is re-armed by every status file, and its expiry is the only unconditional exit.

#### `OperationImpl::OperationImpl`( ioContext, configuration, handler, targetAddress, targetId, dlpTimeout, portOption ) — `host/implementation/OperationImpl.cpp:50`

Stores all six parameters as `const` members, constructs a TFTP client and server from the shared `io_context`, and constructs the timer. The one active step is configuring the server:

```cpp
( *tftpServerV )
.serverAddress( { configurationV.localInterfaceAddress,
portOptionV ? 0U : configurationV.tftpConfiguration.tftpServerPort } )
.requestHandler( std::bind_front( &OperationImpl::receivedTftpRequest, this ) );
```

**Port 0 when the Port Option is enabled** — the OS picks an ephemeral port, whose actual value is read back later via `tftpServerV->localEndpoint().port()` and advertised to the target inside the TFTP option set. That is the whole mechanism of the ARINC 615A-3 Port Option.

#### `void` OperationImpl::initialise( Files::ProtocolFileType fileType ) — `host/implementation/OperationImpl.cpp:249`

Starts the operation proper. In order: `tftpServerV->start()` so the host is listening *before* it asks for anything; allocate a `MemoryFile` as the sink; create a TFTP read operation; configure it by fluent chain; call `request()`; bump `ProtocolFileStatistic::globalReceive()`.

The configuration chain is where the whole `Arinc615aConfiguration` reaches the wire — `tftpTimeout`, `tftpRetries`, `dally`, `optionsConfiguration`, `dlpRetries`, the three handlers, the filename from `protocolFilename( fileType )`, the port option, and the remote endpoint built from `targetAddress()` and `tftpServerPort`.

Catches `Arinc615aException` and only logs it. Note what that means: an exception here leaves the operation with no pending work and no `finished()` call, so the wizard sits on its status page until the DLP timer fires and delivers `finished()`.

#### `void` OperationImpl::initialisationFileCompleted( fileType, MemoryFilePtr, TransferStatus ) — `host/implementation/OperationImpl.cpp:376`

The gate in the diagram above. Resets `initialisationOperationV`, then:

1. Any status other than `Successful` → `finished( OperationAbortedByDlp, "Initialisation File could not be received" )`.
1. Log the raw bytes through the protocol file logger, then decode as `Files::InitializationFile`.
1. Invoke `handlerV.initialisationResponse( initFile.response() )` — in this repository that is `<Operation>::initialisationResponse()`, which emits `operationInitialised` to the wizard.
1. Switch on the acceptance code:

- `OperationAccepted` → **store `protocolVersionV` from the file**, then `triggerDlpTimeout()`. Every protocol file the host writes afterwards uses this version.
- `OperationDenied` / `OperationNotSupported` → `finished()` with that code and the target's description.
- anything else → `finished( OperationAbortedByDlp, "unknown status code" )`.

A decode failure is caught and mapped to `finished( OperationAbortedByDlp, "Initialisation File could not be received" )`.

#### `bool` OperationImpl::initialisationFileOptionsNegotiation( const Arinc615aOptions& ) — `host/implementation/OperationImpl.cpp:342`

Strict validation of what the target echoed back. Returns `false` — which aborts the transfer — in four cases: a checksum option arrived when none was requested; a port option arrived when the port option was disabled in settings; the echoed port differs from the host's actual listening port; or the host requested the port option and the target did not echo it at all.

That last case logs `"Port option not accepted - Operation must be restarted with default port"`, which is the actionable message: clear **Use port option** in the settings dialogue for targets that predate ARINC 615A-3.

#### `void` OperationImpl::triggerDlpTimeout( seconds exceptionTime = 0s ) — `host/implementation/OperationImpl.cpp:149`

One line of policy: `timerV.expires_after( std::max( exceptionTime, dlpTimeoutV ) )`, then re-arm `timerHandler`. The target's advertised exception timer can only ever *extend* the window, never shorten it below the configured DLP timeout. Calling `expires_after` on a pending timer cancels it, which is what makes each status file reset the watchdog.

#### `void` OperationImpl::timerHandler( const error_code& ) — `host/implementation/OperationImpl.cpp:312`

Returns on `operation_aborted` (the re-arm path). A genuine timer error yields `finished( OperationAbortedByDlp, "Internal timer error" )`. Expiry yields `finished( OperationAbortedByDlp, "DLP Timeout" )`.

Carries a `//! @todo cancel active transfers`. In-flight TFTP transfers are *not* cancelled on DLP timeout; the server is stopped by `finished()`, but individual operations unwind on their own schedule.

#### `bool` OperationImpl::isAborted( const udp::endpoint& remote ) — `host/implementation/OperationImpl.cpp:186`

Called at the top of every status-file request handler — the designated moment to inject an abort. Returns `false` immediately if `abortReasonV == NoAbort`. Otherwise it maps the reason to a status code (`Operator` → `OperationAbortedByOperator`, `Protocol` → `OperationAbortedByDlp`), sends it via `tftpServerV->abortOperation( remote, statusCode )`, **resets `abortReasonV` to `NoAbort`**, re-arms the DLP timeout, and returns `true`.

The reset is deliberate: the abort is delivered exactly once, and the host then waits for the target to close the operation with its own final status file. That is why pressing **Abort Operation** once does not close the wizard immediately.

#### `bool` OperationImpl::checkRequest( string_view filename, const udp::endpoint& remote ) — `host/implementation/OperationImpl.cpp:228`

The host's only authorisation check. If the filename parses as a protocol filename *and* its embedded Target ID differs from this operation's, the request is refused with TFTP error `FileNotFound` and the message `"Wrong target ID for protocol file"`. Non-protocol filenames — data files during upload and download — pass through unchecked.

#### `void` OperationImpl::finished( StatusCode status, string_view description = {} ) — `host/implementation/OperationImpl.cpp:159`

Three statements: `handlerV.finished( status, description )`, `timerV.cancel()`, `tftpServerV->stop()`. No idempotence guard, no state check. Every re-entry counts the command's latch down again. Two paths can reach it twice; see [CODE-TRACE.md § 21](CODE-TRACE.md).

#### `void` OperationImpl::doAbort( AbortReason ) · void doTerminate( AbortReason ) — `host/implementation/OperationImpl.cpp:77, 96`

`doAbort` is idempotent — it returns early if `abortReasonV != NoAbort`. It records the reason and, if the initialisation read is still in flight, calls `gracefulAbort()` on it. It does **not** call `finished()`; delivery is deferred to the next `isAborted()` check.

`doTerminate` has no such guard. It aborts the initialisation transfer if present, maps the reason to a status code, and calls `finished()` directly. Two terminate requests in quick succession therefore reach `finished()` twice.

#### `string` OperationImpl::protocolFilename( ProtocolFileType ) const — `host/implementation/OperationImpl.cpp:171`

`static_cast<std::string>( Files::ProtocolFilename{ targetIdV, fileType } )` — the single place protocol filenames are built. See § 2 below for the extension table.

---

---

## 2 — Protocol file codec

Every ARINC 615A protocol file shares a six-byte header and is built from three primitive encodings: big-endian integers, length-prefixed NUL-terminated strings, and three-character ASCII ratios. All of it lives under `lib/arinc_615a/files/`. The length field is redundant with the transfer size but is validated strictly, which makes truncated protocol files fail fast rather than decode into garbage.

#### `void` ProtocolFile::insertHeader( RawDataSpan ) const · ConstRawDataSpan decodeHeader( ConstRawDataSpan ) — `files/ProtocolFile.cpp:43, 52`

`insertHeader` is called *last* by every `encode()` — the body is built first, then the now-known total length and the version are written into the reserved leading six bytes.

`decodeHeader` enforces three things and throws `Arinc615aException` on each: the buffer is at least `HeaderSize`; the embedded length equals the actual buffer size (`"internal length field and data size differs"`); the version is one of `Arinc615a2` (0x4133) or `Arinc615a34` (0x4134). **`Arinc615a1` (0x4132) is deliberately rejected** — supplement 1 protocol files are not supported.

#### `RawData` String_encode( string_view, uint8_t fixedLength = 0 ) · tuple<span,string_view> String_decode( ConstRawDataSpan ) — `files/String.cpp:64, 24`

`String_encode` computes `rawStringSize` as zero for an empty string, otherwise `size() + 1` for the terminator. Throws if that reaches 255 or exceeds `fixedLength` when one is given. Writes the length byte, then the characters, then forces the final byte to NUL.

`String_decode` reads the length byte, validates the remaining buffer is long enough (`"string length inconsistent"`), and for non-zero lengths requires an embedded NUL (`"string not NULL terminated"`). The returned string is truncated at that NUL — **a declared length longer than the actual text is legal**, which is exactly the ARINC 615A-2 change noted in the [README](../README.md#protocol-background).

#### `tuple<span,Ratio>` Ratio_decode( ConstRawDataSpan ) · RawData Ratio_encode( const Ratio& ) — `files/Ratio.cpp`

Ratios are three ASCII digits, space-padded via `std::format( "{:3}", value )`. Decode parses with `std::stoul` and rejects values above 100. A parse failure becomes `Arinc615aException`. These carry upload and download completion percentages in status files.

#### Filename mapping

`ProtocolFilename` splits on the first `.`, validates the stem as a Target ID, and maps the extension through a Boost.MultiIndex table at `files/ProtocolFilename.cpp:148`. Both directions are supported, which is why `isProtocolFilename()` can reject unknown extensions cheaply.

| Ext | ProtocolFileType | Operation | Direction |
| --- | --- | --- | --- |
| LCI | LoadConfigurationInitialization | Information | host reads |
| LCL | LoadConfigurationList | Information | target writes |
| LCS | LoadConfigurationStatus | Information | target writes |
| LUI | UploadInitialization | Upload | host reads |
| LUR | UploadRequest | Upload | host writes |
| LUS | UploadStatus | Upload | target writes |
| LND | MediaDefinedDownloadInitialization | Med download | host reads |
| LNR | MediaDefinedDownloadRequest | Med download | host writes |
| LNO | OperatorDefinedDownloadInitialization | Op download | host reads |
| LNL | OperatorDefinedDownloadList | Op download | target writes |
| LNA | OperatorDefinedDownloadAnswer | Op download | host writes |
| LNS | DownloadStatus | both downloads | target writes |

#### File bodies

| Class | Body layout after the 6-byte header |
| --- | --- |
| InitializationFile | uint16 acceptance code, then description string |
| LoadConfigurationListFile | uint16 THW count; per THW: literal name, serial number, uint16 P/N count; per P/N: part number, amendment, part designation |
| InformationOperationStatusFile | uint16 counter, uint16 status code, uint16 exception timer, int16 estimated time, description string |
| DownloadOperationRequestFile | uint16 file count, N filename strings, uint8 UDD length, UDD bytes |
| UploadOperationRequestFile | uint16 load count; per load: header filename, part number |
| UploadOperationStatusFile | counter, status, ratio, exception timer, estimated time, description, then per-load status blocks |

Every `decode()` ends with a check that no bytes remain (`"More data then expected"`), so a target sending vendor extensions past the defined fields will have its file rejected outright. Relaxing that check means editing the core, which is shared with the CLI tool suite — do it upstream, not here.

#### `void` ProtocolFileLogger::logProtocolFile( prefix, filename, file ) — `files/ProtocolFileLogger.cpp`

Enabled by the `protocolFileLogging` flag in `Arinc615aConfiguration`. Writes every protocol file, in both directions, as a raw binary dump named `{ISO-8601 timestamp}_{operation}_{RX|TX}_{protocol filename}` into `loggingDirectoryV`. The directory defaults to the process working directory — nothing in this repository calls `loggingDirectory()`, so files land wherever the executable was started from.

This is the single most useful diagnostic in the tool. When a target rejects a file or the host rejects a decode, the exact bytes are on disk.

---

---

## 3 — What ARINC 615A adds to TFTP

`lib/arinc_615a/tftp/` is a decorator over the generic `tftp` dependency. It adds three things and nothing else: extra option names, two overloaded uses of the TFTP error packet, and a retry layer above the TFTP retry layer.

#### Option names on the wire

`Arinc615aOptions_name()` at `tftp/Arinc615aOptions.cpp` is the entire mapping. These strings go into the TFTP option-negotiation fields verbatim:

| Wire name | Meaning | Check value type |
| --- | --- | --- |
| port | ARINC 615A-3 Port Option — host's dynamic TFTP server port | — |
| part number | Load part number for a data transfer (note the space) | — |
| checksum_1 | CRC8 | Crc8 |
| checksum_2 | CRC16 | Crc16 |
| checksum_3 | CRC32 | Crc32 |
| checksum_4 | MD5 | Md5 |
| checksum_5 | SHA1 | Sha1 |
| checksum_6 | SHA256 | Sha256 |
| checksum_7 | SHA512 | Sha512 |
| checksum_8 | CRC64 | Crc64 |

`Arinc615aOptions_checksum()` extracts whichever checksum option is present, and returns `{ false, {} }` — a negotiation failure — if **more than one** checksum option appears, or if the value fails to parse into a valid `Arinc649::CheckValue`. Exactly zero or one is legal.

#### Error packets carry protocol semantics

ARINC 615A overloads the TFTP error packet, with error code `NotDefined` and a structured message, to carry two protocol events. `ErrorMessage_type()` classifies them:

| Message | Meaning | Host reaction |
| --- | --- | --- |
| WAIT:<seconds> | Target is busy; retry after the stated delay | Arm a timer, call `operationDeferredHandler`, retry **without** consuming a retry |
| ABORT:<4 hex digits> | Target is terminating with this ARINC 615A status code | Map to `OperationAbortedByDlp` or `…ByOperator` and complete |

`ErrorMessage_abort()` parses the four hex digits with `std::from_chars` base 16 and validates the result against the known status codes, returning `StatusCode::Invalid` for anything unrecognised. `ErrorMessage_wait()` parses base 10 into a `uint16_t` and returns `std::nullopt` on failure. Both are `noexcept`; malformed messages degrade to "not an ARINC 615A message" rather than throwing.

#### The DLP retry layer
The teal path is what makes WAIT different from every other failure: it re-issues the request after the target's stated delay without spending a retry, so a busy target cannot exhaust the counter.

#### `void` Tftp::Clients::OperationImpl::handleCompletion( ::Tftp::TransferStatus status ) — `tftp/clients/implementation/OperationImpl.cpp:155`

The function the diagram describes. Success clears the operation and error information and reports `TransferStatus::Successful`. `Aborted` reports a local abort. `RequestError` is classified as ABORT, WAIT, or neither.

ABORT handling is conditional on `handleAbortV`, and only two status codes are honoured — `OperationAbortedByDlp` and `OperationAbortedByOperator`. Any other abort code logs `"Invalid ABORT status code"` and **falls through to the retry path**, which is why the diagram has that long return edge.

Everything that reaches the bottom increments `retriesV` and re-issues via `tftpOperation()` until `retriesV > dlpRetriesV`, at which point it reports `CommunicationError`.

#### `bool` Tftp::Clients::OperationImpl::handleOptionNegotiation( ::Tftp::Packets::Options& serverOptions ) — `tftp/clients/implementation/OperationImpl.cpp:115`

Destructively extracts the ARINC 615A options from the server's option map — port, part number, checksum — leaving only generic TFTP options behind. Anything left over that the generic TFTP layer does not recognise will abort the transfer, which is how unknown options are rejected without an explicit check here.

The assembled `Arinc615aOptions` is then handed to the operation-specific `optionNegotiationHandlerV` — the strict validators in § 1 above.

#### `::Tftp::Clients::OperationPtr` ReadOperationImpl::tftpOperation() — `tftp/clients/implementation/ReadOperationImpl.cpp`

Called once per attempt — including every retry — so each retry rebuilds the underlying TFTP operation from scratch. Assembles `additionalOptions` from the port, part number, and checksum if set, then configures the real TFTP read with `TransferMode::OCTET`, the two ARINC 615A handlers, the data sink, and a local endpoint bound to port 0.

---

---

## 4 — Timers and abort

Timeouts nest. Understanding which layer fired is most of diagnosing a stalled load.

| Timer | Default | Scope | Where it is set |
| --- | --- | --- | --- |
| TFTP packet timeout | 2 s | One TFTP packet; retried `tftpRetries` times | `tftpConfiguration` |
| DLP retry | 1 | Whole TFTP transfer, re-issued from scratch | `dlpRetries` |
| DLP timeout | 13 s | Watchdog on the whole operation; re-armed by each status file | `dlpTimeout` |
| FIND receive window | 3 s | FIND only; fixed listening period | `findTimeout` |

The DLP timeout is the only one that ends an operation unconditionally. Its effective value is `max( exceptionTimer, dlpTimeout )`, so a target that advertises a long exception timer extends the window but can never shrink it below the configured value. Defaults are declared in `Arinc615a.hpp` as `DefaultArinc615aTftpTimeout`, `DefaultArinc615aTftpRetries`, `DefaultArinc615aDlpTimeout`, and `DefaultArinc615aDlpRetries`. > **Documentation mismatch.** Upstream's manpage states a DLP retry default of 2; the source constant `DefaultArinc615aDlpRetries` is `1`. The source is authoritative.

#### Abort versus terminate

- **Abort** (the wizard's **Abort Operation** button, or `AbortReason::Protocol` raised internally) records the reason and waits. Delivery happens at the next status-file request via `isAborted()`, which sends an ARINC 615A ABORT error and lets the target close the operation with its own final status. Idempotent.
- **Terminate** calls `finished()` directly with a locally synthesised status. Not idempotent. No GUI control is wired to it; only `DataLoaderMainWindow`'s shutdown path reaches `terminate()`.

If the target has gone silent, abort never delivers, and the operation ends on the DLP timeout instead. There is no faster escape from the GUI — closing the wizard does not cancel a transfer in flight.

---

---

## 5 — Status codes

Declared in `Arinc615a.hpp`. These values appear in status files, in initialisation responses, and inside `ABORT:` error messages.

| Value | Enumerator | Meaning | Terminal |
| --- | --- | --- | --- |
| 0x0001 | OperationAccepted | Accepted, not yet started | no |
| 0x0002 | OperationInProgress | Running; exception timer valid | no |
| 0x0003 | OperationCompleted | Completed without error | yes |
| 0x0004 | OperationInProgressAdditionalInfo | Running, with description text (615A-3+) | no |
| 0x1000 | OperationNotAccepted | Denied — initialisation response only | yes |
| 0x1002 | OperationNotSupported | Not supported — initialisation response only | yes |
| 0x1003 | OperationAbortedByTargetHw | Target aborted | yes |
| 0x1004 | OperationAbortedByDlp | Data loader aborted | yes |
| 0x1005 | OperationAbortedByOperator | Operator aborted | yes |
| 0x1007 | LoadPartNumberOrDownloadFileFailed | Per-file or per-load failure inside a status file | per item |
| 0xFFFE | OperationDeferred | **Internal only** — signals a WAIT response, never on the wire | no |
| 0xFFFF | Invalid | Sentinel for parse failure | — |

`statusCode( uint16_t )` at `StatusCode.cpp` validates on decode and **throws `Arinc615aException`** for any value not in this table — including `OperationDeferred`, which is why that code can never legally arrive from a target. `status( OperationClass, StatusCode, description, … )` renders the human-readable sentences used in log output.

---

---

---

## Provenance

`lib/arinc_615a` is upstream code by Thomas Vogt, MPL-2.0, carried here
unmodified — see [CONTRIBUTING.md](../CONTRIBUTING.md) for what that means when
you are tempted to change it.

Parts of this document were written while tracing the same core from the
command-line side and are reproduced here, adapted to the GUI, so that this
repository is complete on its own. Where behaviour is described in terms of a
control — the **Abort Operation** button, the settings dialogue — that is this
application; the underlying core call is identical either way.

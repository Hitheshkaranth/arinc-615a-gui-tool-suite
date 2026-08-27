# Architecture

How the graphical data loader is put together, and why. Read this before
changing code.

The protocol core is not described here beyond what a GUI author needs — for the
wire layers, see the CLI repository's
[docs/ARCHITECTURE.md](https://github.com/Hitheshkaranth/arinc-615a-cli-tool-suite/blob/main/docs/ARCHITECTURE.md)
and `docs/CODE-TRACE.md` §09–§17. That code is shared and unmodified.

---

## 1. The one idea

Everything in this repository exists to solve a single mismatch:

> The protocol core is **callback-driven and thread-agnostic**. Qt is
> **signal-driven and thread-affine**. Widgets may only be touched by the thread
> that created them.

The core calls your handler from whichever thread is running its `io_context`.
Qt forbids touching a widget from that thread. The `operations/` layer is the
adapter that reconciles the two, and every design decision below follows from it.

---

## 2. Layer map

```
  ┌──────────────────────────────────────────────────────────────────┐
  │  app/arinc_615a_data_loader_gui                                  │
  │  register 3 resource archives · QApplication · show main window  │
  └──────────────────────────────────────────────────────────────────┘
                                 │
  ┌──────────────────────────────────────────────────────────────────┐
  │  arinc_615a_dla_qt :: DataLoaderMainWindow                       │
  │  owns io_context · work guard · I/O thread · FIND client ·       │
  │  host protocol · configuration · target list · media sets        │
  └──────────────────────────────────────────────────────────────────┘
                                 │
  ┌──────────────────────────────────────────────────────────────────┐
  │  arinc_615a_dla_qt :: wizards                                    │
  │  find/ · information/ · upload/ · download/                      │
  └──────────────────────────────────────────────────────────────────┘
                                 │
  ══════════════════ THREAD BOUNDARY ════════════════════════════════
                                 │
  ┌──────────────────────────────────────────────────────────────────┐
  │  arinc_615a_dla_qt :: operations                                 │
  │  FindQuery · InformationOperation · UploadOperation ·            │
  │  MediaDefinedDownloadOperation · OperatorDefinedDownloadOperation│
  └──────────────────────────────────────────────────────────────────┘
                                 │
  ┌──────────────────────────────────────────────────────────────────┐
  │  arinc_615a  (core, no Qt)                                       │
  │  Host::Protocol · Find::Clients::Client · protocol file codec ·  │
  │  TFTP + 615A options                                             │
  └──────────────────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────────────────┐
  │  arinc_615a_qt   (side library, GUI thread only)                 │
  │  12 QAbstractTableModel subclasses · 2 dialogues · status codes  │
  └──────────────────────────────────────────────────────────────────┘
```

Control flows downward. Results climb only through Qt signals or handler
interfaces bound at construction. No layer calls back into the layer above it
directly.

---

## 3. The thread boundary

Two threads, for the whole life of the process.

| Thread | Started by | Runs | Owns |
| --- | --- | --- | --- |
| **GUI** | the OS, as `main` | `QApplication::exec()` | every widget, model, wizard |
| **I/O** | `DataLoaderMainWindow` constructor, last statement | `ioContext.run()` | every socket; all protocol callbacks |

### Lifetime

`boost::asio::executor_work_guard` is constructed in the member init list,
*before* the thread starts, so `run()` does not return when the queue drains
between operations. Shutdown is strictly ordered in the destructor:

```cpp
workGuard.reset();   // let run() return
ioThread.join();     // no more callbacks after this point
saveConfiguration(); // only now is it safe to touch state the I/O thread used
```

Getting that order wrong produces a use-after-free that only shows up under
load, so do not reorder it.

### Crossing it

Every connection from an adapter signal to a wizard slot names
`Qt::QueuedConnection` explicitly:

```cpp
connect( operationV.get(), &InformationOperation::receivedStatus,
         ui->status,       &InformationOperationStatusPage::operationStatus,
         Qt::QueuedConnection );
```

`Qt::AutoConnection` would resolve to the same thing — it compares the emitting
thread against the receiver's affinity at emit time. Naming it is documentation,
and it stops a later refactor from silently turning a cross-thread call into a
direct one.

Queued delivery **copies** its arguments, so:

1. Every payload type is registered with `qRegisterMetaType` in the adapter
   constructor.
2. Borrowed types must be materialised. The handler interface hands
   `finished()` a `std::string_view`; the signal takes `const std::string &`,
   and the adapter body is `emit operationFinished( code, std::string{ description } )`.
   A view into a buffer owned by the I/O thread would dangle before the GUI
   thread dequeued it.

### The rule

> **Never touch a widget from a handler callback.** Emit a signal.

Handler callbacks — `initialisationDeferred`, `initialisationResponse`,
`finished`, `status`, `targetInformation`, `fileRequest` — all run on the I/O
thread. Slots — `startOperation`, `abortOperation`, `transmitLoads`,
`transmitFiles` — run on the GUI thread and may use widgets freely.

Three places currently break this rule; see [CODE-TRACE.md](CODE-TRACE.md) § 21.

---

## 4. The adapter pattern

Every operation adapter has the same shape:

```cpp
class InformationOperation final :
  public  QObject,
  private Arinc615a::Host::InformationOperationHandler
{
    Q_OBJECT
    // ...
};
```

**Public `QObject`, private handler interface.** The private inheritance is
deliberate: the handler methods are an implementation detail that only the
protocol core may call, and it acquires that right through `.handler = *this`
in the operation configuration. Nothing in the GUI can invoke them.

The mapping is one-for-one:

| Handler method (I/O thread) | Signal (queued to GUI thread) |
| --- | --- |
| `initialisationDeferred( seconds )` | `operationDeferred( seconds )` |
| `initialisationResponse( response )` | `operationInitialised( response )` |
| `finished( code, string_view )` | `operationFinished( code, std::string )` |
| `status( status )` | `receivedStatus( status )` |
| `targetInformation( hw, integrity )` | `receivedInformation( hw, integrity )` |

Most bodies are exactly one line. The adapter holds no state machine of its own
— it is a thread-boundary marshaller, and it should stay that way.

`startOperation()` is the one slot that does real work: it builds the core's
designated-initialiser configuration struct and calls `start()`. That struct is
identical to the one the CLI builds from `boost::program_options` values.

---

## 5. Directory walkthrough

### `app/arinc_615a_data_loader_gui/`
Roughly 90 lines. Registers three Qt resource archives, constructs
`QApplication`, sets organisation name and domain (which is what `QSettings`
and the configuration location key off), shows the window. No protocol logic.
Linked `WIN32`, so there is no console — the log file is the only diagnostic.

### `lib/arinc_615a_dla_qt/`
The application library.

| Path | Contents |
| --- | --- |
| `DataLoaderMainWindow.*` | The shell. Owns everything with process lifetime. |
| `Configuration.*` | Load and save `DataLoader.json` and `Targets.json`. |
| `DataLoaderConfiguration.*` | Plain struct, property-tree serialisable, embeds the core's own configuration objects. |
| `SettingsDialog.*` | Edits that struct. |
| `SelectTargetWidget.*` | Shared target-picker used by four wizards. |
| `operations/` | The five adapters. **The thread boundary lives here.** |
| `find/`, `information/`, `upload/`, `download/` | One directory per wizard: the wizard, its pages, and their `.ui` files. |
| `resources/` | SVG icon set plus `.qrc`, compiled in via `CMAKE_AUTORCC`. |

### `lib/arinc_615a_qt/`
Deliberately a separate library. Depends only on `arinc_615a` and
`Qt::Widgets`, holds no application state, and is reusable by any Qt program
that needs to display ARINC 615A types. Twelve `QAbstractTableModel`
subclasses, two dialogues, and a status-code translation helper.

The three *status log* models are **append-only**: each status file the target
sends becomes a row rather than replacing the previous one, which gives the
operator the whole history of an operation. The CLI has no equivalent — its
status output is a stream.

### `lib/arinc_615a/`
The protocol core, vendored unchanged. No Qt dependency. Do not add one.

---

## 6. Wizard conventions

Every operation wizard is a `QWizard` laid out in Qt Designer with its pages as
promoted widgets. The constructor does seven things, in this order:

1. Construct the adapter as a **child** `QObject`, so it dies with the wizard.
2. `ui->setupUi( this )`.
3. Push context onto the settings page — target list, and media sets or a
   download directory where relevant.
4. `setButtonText( CustomButton1, tr( "Abort Operation" ) )`.
5. Set the operation's SVG as `LogoPixmap` at 64 px on **every** page, by
   iterating `pageIds()`.
6. Connect settings-page selections up, status-page start/abort down.
7. Connect each adapter signal to the pages that render it, always queued.

Step 7 routinely fans one signal out to several receivers — in
`InformationOperationWizard`, `operationFinished` reaches the wizard, the status
page and the completed page. No receiver knows about the others.

### Page semantics

| Page | Role | Commit page? |
| --- | --- | --- |
| Settings / Select address | Target and parameters; gates *Next* via `isComplete()` | Yes |
| Select files | Operator-defined download only — names the files to request | Yes |
| Status | Live status log, ratio, exception timer, per-file or per-load table | — |
| Completed | Final status code and description, part numbers or file list | — |

Marking the settings page a commit page makes the operation irreversible at the
right moment: after *Commit* the wizard cannot go back, matching an operation
that has already put a packet on the wire.

Once live, `initializePage()` sets `QWizard::NoCancelButton` and enables
`HaveCustomButton1`. *Cancel* is the wrong verb for a transfer in progress;
*Abort Operation* is wired to the protocol's abort request, which only sets a
flag — the request goes out with the next status file the target asks for.

### Wizards are modeless and self-disposing

```cpp
auto * const wizard{ new FindQueryWizard{ findClient, configuration, targetsV, this } };
connect( wizard, &FindQueryWizard::finished, wizard, &FindQueryWizard::deleteLater );
connect( wizard, &FindQueryWizard::finished, this,   &DataLoaderMainWindow::updatePacketStatistic );
wizard->show();
```

`show()`, not `exec()` — several operations can be open at once and the main
window stays live behind them.

---

## 7. Design conventions worth knowing

**Statistics refresh on wizard close, not on a timer.** The six counters in the
main window are process-global objects written by the I/O thread. Polling them
from a timer would read them concurrently with those writes, so instead
`updatePacketStatistic()` is connected to each wizard's `finished` signal. The
consequence is that the main window is a post-hoc summary, not a live monitor —
during a long upload, the wizard's own tables are the only live feedback.

**Upload is gated on the media set scan.** `loadMediaSetManager()` disables
*Upload Operation* and *Manage Media Sets*, then defers the scan with
`QMetaObject::invokeMethod( ..., Qt::QueuedConnection )` so it starts after the
constructor returns and the window is visible. Both completion paths call
`deleteLater()` on the action and its progress dialogue.

**Generated sources are exempt from clang-tidy.** Both Qt libraries write a stub
into their binary directory:

```cmake
file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/.clang-tidy "Checks: '-*,llvm-twine-local'" )
```

This suppresses the checker on AUTOMOC and AUTOUIC output, which is
machine-generated and would otherwise flood the report.

**Qt libraries return early when Qt is absent.** Each `*_qt` CMakeLists opens
with `find_package( Qt6 ... QUIET )` and returns if the target is missing. In
*this* repository Qt is mandatory and `find_package( Qt6 REQUIRED ... )` is in
the top-level `CMakeLists.txt`, but the sibling projects keep the guard so they
can be consumed by CLI-only builds.

---

## 8. Where to change things

| You want to… | Go to |
| --- | --- |
| Add a column to a status table | `lib/arinc_615a_qt/*Model.cpp` — `columnCount`, `headerData`, `data` |
| Change what a wizard page shows | `lib/arinc_615a_dla_qt/<operation>/*Page.cpp` and its `.ui` |
| Add a new protocol operation | A new adapter in `operations/`, a new wizard directory, an action in `DataLoaderMainWindow.ui` |
| Change a default timeout or port | `DataLoaderConfiguration` — and `SettingsDialog` to expose it |
| Change protocol behaviour | **Not here.** `lib/arinc_615a/`, shared with the CLI |
| Add an icon | `lib/arinc_615a_dla_qt/resources/` plus the `.qrc` |

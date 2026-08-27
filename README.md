# ARINC 615A Data Loader (GUI)

A Qt 6 host data loader for the ARINC 615A Data Loading Protocol. This is the **graphical section**
of the ARINC 615A Tool Suite, extracted into a standalone project — the command-line section
(`arinc_615a_commands`, `arinc_615a_operation`) is not part of this repository.

![ARINC 615A Data Loader main window](doc/images/main-window.png)

The main window is a live view of the protocol stack: TFTP packet counters, ARINC 615A FIND packet
counters, and ARINC 615A protocol file counters, each split into receive and transmit. Operations
themselves run in modeless wizards launched from the toolbars.

## What it does

Four ARINC 615A operations, each as a wizard:

| Operation | Wizard pages | Purpose |
|---|---|---|
| **FIND query** | Select address → Results | Discover loadable targets by IPv4 broadcast |
| **Information** | Settings → Status → Completed | Read target hardware part numbers and versions |
| **Upload** | Settings → Status → Completed | Send ARINC 665 loads to a target |
| **Media defined download** | Settings → Status → Completed | Retrieve host-named files from a target |
| **Operator defined download** | Settings → Select files → Status → Completed | Retrieve files the target advertises |

It also embeds the ARINC 665 media set manager, which is what supplies the loads an upload transfers.

## Layout

```
lib/arinc_615a          ARINC 615A protocol core — shared with the CLI tool suite
lib/arinc_615a_qt       Qt models and dialogues over ARINC 615A types (12 models, 2 dialogues)
lib/arinc_615a_dla_qt   Data loader application library — main window, 5 adapters, 4 wizards
app/arinc_615a_data_loader_gui   Application entry point (~90 lines, no protocol logic)
```

The core is vendored rather than fetched because the graphical section cannot build without it.
Everything above it is Qt; the core itself has no Qt dependency and is byte-for-byte the same code
the command-line tool suite runs — a packet capture of an operation is indistinguishable between
the two front ends.

### How the layers talk

Classes in `lib/arinc_615a_dla_qt/operations/` implement the core's handler interfaces **privately**
and re-emit every callback as a Qt signal. That is the whole thread boundary: protocol callbacks
arrive on an ASIO I/O thread, and every connection into the GUI is an explicit
`Qt::QueuedConnection`. Payload types are registered with `qRegisterMetaType` so queued delivery can
copy them — which is also why `finished()`'s `std::string_view` becomes a `std::string` on the
signal side.

## Dependencies

Fetched at configure time from `git.thomas-vogt.de`: `helper`, `qt_icon_resources`, `arinc_649`,
`arinc_665`, `tftp`, `commands`. Supplied by vcpkg: Boost, libxml++, spdlog. Qt 6 is required and is
**not** taken from vcpkg — see below.

The About dialogue reports exactly what a given build was linked against:

![About dialogue showing dependency versions and licences](doc/images/about-dialog.png)

## Building

| Tool | Version exercised |
|---|---|
| MSVC | 14.44.35207 (VS 2022 Build Tools) |
| CMake | 4.3.4 |
| Qt | 6.8.3 `msvc2022_64` |
| Boost | 1.92 (vcpkg, `x64-windows`) |
| Generator | Ninja |

### Qt

The vcpkg manifest offers a `with-qt` feature that compiles `qtbase` and `qtsvg` from source — hours
of build time. The official prebuilt binaries take about 80 seconds:

```sh
python -m pip install aqtinstall
python -m aqt install-qt windows desktop 6.8.3 win64_msvc2022_64 -O <prefix>/Qt
```

Do not pass `-m qtsvg`; for 6.8.3 it fails with *"The packages ['qtsvg'] were not found"*. Qt SVG is
already in the base package and `Qt6Svg` appears in `lib/cmake` without it. The SVG module is not
optional at runtime — without its `qsvg` image-format plugin the application starts but every icon
renders blank.

### Configure and build

```sh
configure-gui.bat
build-gui.bat
deploy-qt.bat     # once, before the first launch
run-gui.bat
```

The scripts source `vcvars64.bat` and resolve the toolchain through `env-gui.bat`. Three variables
override the defaults if you set them before calling:

| Variable | Default | Why it may need setting |
|---|---|---|
| `CMAKE_EXE` | bundled CMake 4.3.4, else `cmake` on `PATH` | The project requires CMake 4.3; Visual Studio ships 3.31, and `vcvars64.bat` puts it first on `PATH` |
| `VCPKG_ROOT` | sibling `.tools/vcpkg` | `vcvars64.bat` injects its own `VCPKG_ROOT`, which `env-gui.bat` deliberately overrides |
| `QT6_DIR` | `../Qt/6.8.3/msvc2022_64` | Wherever aqtinstall put Qt |

Doing it by hand is the same two commands with those variables exported:

```sh
cmake --preset msvc-static-debug-gui
cmake --build --preset msvc-static-debug-gui
```

Warnings-as-errors is off in this preset: AUTOMOC and AUTOUIC generated translation units and the Qt
headers do not survive `/W4 /WX` with this toolchain.

## Running

Two sets of runtime libraries are missing from a fresh build tree.

**Qt** — deploy them next to the executable:

```sh
"$QT6_DIR/bin/windeployqt.exe" --debug --no-translations --no-opengl-sw \
  cmake-build-msvc-static-debug-gui/app/arinc_615a_data_loader_gui/arinc_615a_data_loader_gui.exe
```

`deploy-qt.bat` wraps this.

**vcpkg** — the preset sets `VCPKG_APPLOCAL_DEPS=OFF`, so put
`cmake-build-msvc-static-debug-gui/vcpkg_installed/x64-windows/debug/bin` on `PATH`. `run-gui.bat`
does this and launches.

The application logs to `%TEMP%/arinc_615a_data_loader_gui.log` at `info` level and stores
`DataLoader.json` and `Targets.json` in the Qt application configuration location. `Targets.json`
uses the same schema as the CLI tool suite's `--targets-list`, so a target list built in either
front end is readable by the other.

### Verifying the stack

*Targets → FIND Query*, accept the broadcast address, *Commit*. After the receive timeout (3 s by
default) *Finish* enables. Closing the wizard refreshes the main window counters; with no target
hardware on the network the FIND TX table reads:

```
Packet Type            Packet Count   Packet Size
Information Request    1              4
```

One 4-byte FIND Information Request actually transmitted, no responses — the full round trip through
wizard, adapter, core, socket, timer and queued signal.

## Documentation

- **[doc/GUI_BUILD_AND_RUN.md](doc/GUI_BUILD_AND_RUN.md)** — the build and run procedure in
  reproducible form.
- **[doc/generated/arinc615a-gui-engineering.html](doc/generated/arinc615a-gui-engineering.html)** —
  a 22-section engineering trace: architecture, the thread boundary, each operation, the model
  layer, and three thread-affinity defects found while tracing.
- `doc/images/flow/` — screenshots of the FIND wizard flow.

## Known issues

Three code paths construct `QMessageBox` off the GUI thread, violating Qt's thread affinity rules —
the I/O thread body in `DataLoaderMainWindow`, and `finished()` in both download adapters. All three
are error paths, so they do not appear in normal operation. Details in §21 of the engineering trace.

## Licence

This project is licensed under the terms of the
[Mozilla Public License Version 2.0](LICENSE).

Qt 6 is used under **LGPL-3.0-only** and is linked dynamically; it is not distributed as part of this
repository. Fetched dependencies carry their own licences — MPL-2.0 for the `thomas-vogt.de`
libraries, the Boost Software License for Boost, and MPL-2.0 / CC BY 4.0 for `qt_icon_resources`.

## References

- ARINC 615A-4 — Software Data Loader Using Ethernet Interface
- ARINC 665-5 — Loadable Software Standards
- ARINC 649 — Common Terminology and Functions for Software Distribution and Loading

Upstream tool suite: <https://git.thomas-vogt.de/thomas-vogt/arinc_615a>

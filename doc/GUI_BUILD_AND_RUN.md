# Building and Running the GUI Section

This tree ships two independent application sections built from one protocol core:

| Section | Libraries | Application |
|---|---|---|
| Command line | `arinc_615a_commands` | `arinc_615a_operation` |
| Graphical | `arinc_615a_qt`, `arinc_615a_dla_qt` | `arinc_615a_data_loader_gui` |

Both sit on `lib/arinc_615a` — the ARINC 615A protocol core. Nothing in the graphical
section depends on the command-line section, and vice versa.

## Section switches

Two CMake options select what gets configured:

```cmake
option( ARINC_615A_BUILD_CLI "Build the ARINC 615A command-line section" ON )
option( ARINC_615A_BUILD_GUI "Build the ARINC 615A graphical section (requires Qt 6)" OFF )
```

They gate three places:

- `CMakeLists.txt` — `qt_icon_resources` is only declared and fetched when `ARINC_615A_BUILD_GUI`
  is on, so a CLI build pulls five `FetchContent` dependencies instead of six.
- `lib/CMakeLists.txt` — `arinc_615a_commands` under CLI; `arinc_615a_qt` and
  `arinc_615a_dla_qt` under GUI. `arinc_615a` is always added.
- `app/CMakeLists.txt` — `arinc_615a_operation` under CLI; `arinc_615a_data_loader_gui` under GUI.

`InstallPackage.cmake` guards its runtime-dependency directory list with `if( TARGET ... )`
so the `RUNTIME_DEPENDENCY_SET` install works in either configuration.

Configuring with both off is a `FATAL_ERROR`.

## Prerequisites

| Tool | Version used | Notes |
|---|---|---|
| MSVC | 14.44.35207 (VS 2022 Build Tools) | `vcvars64.bat` must be sourced |
| CMake | 4.3.4 | bundled in `.tools/cmake-4.3.4-windows-x86_64` |
| Ninja | from VS Build Tools | picked up by the preset generator |
| vcpkg | `.tools/vcpkg` | supplies Boost, libxml++, spdlog |
| Qt | 6.8.3 `msvc2022_64` | GUI section only |

Qt is **not** taken from vcpkg here. The `with-qt` vcpkg feature would compile `qtbase` and
`qtsvg` from source; the official prebuilt binaries install in about 80 seconds instead:

```sh
python -m pip install aqtinstall
python -m aqt install-qt windows desktop 6.8.3 win64_msvc2022_64 -O <prefix>/Qt
```

Note that `-m qtsvg` is rejected for 6.8.3 — Qt SVG is already part of the base package, and
`Qt6Svg` is present in `lib/cmake` after a plain install.

## Configure and build

The preset `msvc-static-debug-gui` (in `CMakePresetsMsvc.json`) sets both section switches and
reads the Qt prefix from the `QT6_DIR` environment variable:

```json
"cacheVariables": {
  "CMAKE_BUILD_TYPE": "Debug",
  "ARINC_615A_BUILD_CLI": "OFF",
  "ARINC_615A_BUILD_GUI": "ON",
  "CMAKE_PREFIX_PATH": "$env{QT6_DIR}",
  "CMAKE_COMPILE_WARNING_AS_ERROR": "False"
}
```

It writes to `cmake-build-msvc-static-debug-gui/`, so the CLI build tree is left untouched.
Warnings-as-errors is off for this preset because AUTOMOC/AUTOUIC generated sources and the Qt
headers themselves do not survive `/W4 /WX`.

Two wrapper scripts are provided that source `vcvars64.bat` and set `VCPKG_ROOT` and `QT6_DIR`:

```sh
./configure-gui.bat      # cmake --preset msvc-static-debug-gui
./build-gui.bat          # cmake --build --preset msvc-static-debug-gui
```

The build preset restricts the target list to `arinc_615a_data_loader_gui`, which pulls in
exactly what it needs and skips the ARINC 665 GUI applications that `FetchContent` also makes
available.

306 build steps, no errors. The result is
`cmake-build-msvc-static-debug-gui/app/arinc_615a_data_loader_gui/arinc_615a_data_loader_gui.exe`.

### Long-path warning

CMake emits `CMAKE_OBJECT_PATH_MAX` warnings for the three ARINC 665 GUI applications under
`_deps/arinc_665-build/app/...` — their object paths run to 215–219 characters against a 250
limit. They are warnings, not errors, and those targets are not in our build set. Move the
checkout closer to the drive root if you intend to build the full ARINC 665 application set.

## Running

The executable needs two sets of runtime libraries that are not next to it after a build:

1. **Qt** — deploy with `windeployqt`:
   ```sh
   "$QT6_DIR/bin/windeployqt.exe" --debug --no-translations --no-opengl-sw \
     cmake-build-msvc-static-debug-gui/app/arinc_615a_data_loader_gui/arinc_615a_data_loader_gui.exe
   ```
   This copies `Qt6Cored.dll`, `Qt6Guid.dll`, `Qt6Widgetsd.dll`, `Qt6Networkd.dll`,
   `Qt6Svgd.dll` plus the `platforms/`, `styles/`, `imageformats/`, `tls/` and
   `networkinformation/` plugin directories.

2. **vcpkg** — the preset sets `VCPKG_APPLOCAL_DEPS=OFF`, so Boost, libxml++ and spdlog are not
   copied. Put `cmake-build-msvc-static-debug-gui/vcpkg_installed/x64-windows/debug/bin` on
   `PATH`.

`run-gui.bat` does the second part and launches the application.

The application logs to `%TEMP%/arinc_615a_data_loader_gui.log` at `info` level and stores its
configuration (`DataLoader.json`, `Targets.json`) in the Qt application configuration location.

## Verifying the flow

`Targets → FIND Query` opens the FIND wizard. Page one offers the global IPv4 broadcast address
plus every interface broadcast address `QNetworkInterface` reports; committing it moves to the
results page, which starts the query immediately and re-enables **Finish** when the receive
timeout (3 s by default) elapses.

Closing the wizard refreshes the main window statistics. After one query with no target hardware
on the network, the FIND TX table reads:

```
Packet Type            Packet Count   Packet Size
Information Request    1              4
```

— one 4-byte FIND Information Request actually transmitted, no responses. That single row is the
end-to-end confirmation that the GUI drives the same protocol core the CLI does.

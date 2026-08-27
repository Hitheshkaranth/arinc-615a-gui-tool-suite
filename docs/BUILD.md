# Building and running

Every stage in order, what each one needs, and the failure modes that will
otherwise stop you.

---

## 1. What gets built

One executable, `arinc_615a_data_loader_gui`, from three libraries:

| Library | Role |
| --- | --- |
| `lib/arinc_615a` | ARINC 615A protocol core — vendored, no Qt dependency |
| `lib/arinc_615a_qt` | Qt models and dialogues over ARINC 615A types |
| `lib/arinc_615a_dla_qt` | Main window, operation adapters, wizards, resources |

Qt 6 is a hard requirement. The top-level `CMakeLists.txt` declares
`find_package( Qt6 REQUIRED COMPONENTS Widgets Network )`, so a configure that
cannot find Qt fails immediately with a clear message rather than succeeding and
quietly producing nothing.

The command-line section of the tool suite lives in its own repository,
[arinc-615a-cli-tool-suite](https://github.com/Hitheshkaranth/arinc-615a-cli-tool-suite).
Nothing here depends on it.

Roughly 306 Ninja edges are needed to reach the executable, most of them the
fetched sibling projects.

---

## 2. Prerequisites

| Tool | Version exercised | Notes |
| --- | --- | --- |
| MSVC | 14.44.35207 (VS 2022 Build Tools) | `vcvars64.bat` must be sourced |
| CMake | **4.3.4** | every `CMakeLists.txt` declares `cmake_minimum_required( VERSION 4.3 )` |
| Ninja | from VS Build Tools | picked up by the preset generator |
| vcpkg | any recent checkout | supplies Boost, libxml++, spdlog |
| Qt | **6.8.3** `msvc2022_64` | see §3 |
| Python | 3.12 | only to install Qt, and to regenerate the docs |

Only the MSVC presets are provided. See the README's *Platform note*.

---

## 3. Getting Qt

Two routes. Take the second.

**vcpkg `with-qt` feature.** The upstream manifest offers it, and it works — it
compiles `qtbase` and `qtsvg` from source. Expect hours and tens of gigabytes of
build trees.

**Prebuilt binaries via aqtinstall.** About 80 seconds:

```bash
python -m pip install aqtinstall
python -m aqt install-qt windows desktop 6.8.3 win64_msvc2022_64 -O <prefix>/Qt
```

This matches the MSVC 14.44 toolchain the rest of the tree uses.

> **Do not pass `-m qtsvg`.** For 6.8.3 it fails with *"The packages ['qtsvg']
> were not found while parsing XML of package information"*. Qt SVG is already
> part of the base package; after a plain install `Qt6Svg` is present in
> `lib/cmake`. It is not optional at runtime — see §6.

---

## 4. Environment traps

`vcvars64.bat` is not neutral. It changes two things this build depends on, and
neither failure names its cause.

| It does this | You see | Worked around by |
| --- | --- | --- |
| Injects **its own `VCPKG_ROOT`**, pointing at Visual Studio's bundled vcpkg | A fresh 84-package vcpkg build, or missing-package errors | `scripts/env-gui.bat` captures your value first, then restores it after the call |
| Prepends **CMake 3.31** to `PATH`, ahead of the required 4.3 | `CMake 4.3 or higher is required. You are running version 3.31.6-msvc6` | `scripts/env-gui.bat` resolves `CMAKE_EXE` explicitly instead of relying on `PATH` |

Three variables override the script's defaults if you set them beforehand:

| Variable | Default |
| --- | --- |
| `CMAKE_EXE` | bundled CMake 4.3.4, else `cmake` on `PATH` |
| `VCPKG_ROOT` | sibling `.tools/vcpkg` |
| `QT6_DIR` | `..\Qt\6.8.3\msvc2022_64` |

A third, harmless, oddity: `vcvars64.bat` prints `'vswhere.exe' is not
recognized` when `vswhere` is not on `PATH`. It still succeeds. Ignore it.

---

## 5. Configure and build

One command:

```bat
build.bat
```

Configure, build, deploy the Qt runtime, launch. `build.bat --no-run` stops
after deploying; `build.bat --no-deploy` skips `windeployqt`.

Or the steps separately:

```bat
scripts\configure-gui.bat
scripts\build-gui.bat
```

The preset `msvc-static-debug-gui` lives in `cmake/presets/CMakePresetsMsvc.json`
and sets:

```json
"cacheVariables": {
  "VCPKG_APPLOCAL_DEPS": "OFF",
  "CMAKE_PREFIX_PATH": "$env{QT6_DIR}",
  "CMAKE_COMPILE_WARNING_AS_ERROR": "False",
  "CMAKE_BUILD_TYPE": "Debug"
}
```

The build preset restricts targets to `arinc_615a_data_loader_gui`, which keeps
the three ARINC 665 GUI applications — also made available by `FetchContent` —
out of the build.

**Warnings-as-errors is deliberately off.** AUTOMOC and AUTOUIC generated
translation units and the Qt headers do not survive `/W4 /WX` with MSVC 14.44,
and Boost 1.92's exception headers raise C4127 through `/external:templates-`.
Neither has anything to do with this repository's own code.

> Configure **requires network access to `git.thomas-vogt.de`** — six sibling
> projects are cloned by `FetchContent`. There is no vendored copy and no
> offline fallback.

---

## 6. Running

A freshly built executable will not start from the build tree. Two sets of
runtime libraries are missing.

### Qt

```bat
scripts\deploy-qt.bat
```

which runs:

```bash
"$QT6_DIR/bin/windeployqt.exe" --debug --no-translations --no-opengl-sw \
  cmake-build-msvc-static-debug-gui/app/arinc_615a_data_loader_gui/arinc_615a_data_loader_gui.exe
```

It copies `Qt6Cored`, `Qt6Guid`, `Qt6Widgetsd`, `Qt6Networkd`, `Qt6Svgd` and
`D3Dcompiler_47`, plus five plugin directories:

| Directory | Plugin | Consequence if missing |
| --- | --- | --- |
| `platforms` | `qwindowsd` | Application will not start at all |
| `imageformats` | `qsvgd`, `qicod`, `qjpegd` | **Every toolbar icon renders blank** |
| `styles` | `qmodernwindowsstyled` | Falls back to an older visual style |
| `tls` | `qschannelbackendd` | TLS unavailable (unused by 615A itself) |
| `networkinformation` | `qnetworklistmanagerd` | Network state queries unavailable |

Run it once after the first build; repeat only after upgrading Qt.

### vcpkg

The preset sets `VCPKG_APPLOCAL_DEPS=OFF`, so Boost, libxml++ and spdlog are not
copied next to the executable. Put

```
cmake-build-msvc-static-debug-gui\vcpkg_installed\x64-windows\debug\bin
```

on `PATH` — nineteen DLLs, including the Boost `-mt-gd-x64-1_92` set, `libxml2`,
`xml++-vc143-5.0-1` and `spdlogd`. `scripts\run-gui.bat` does this and launches.

---

## 7. Verifying it worked

The application starts with three empty statistic group boxes, a menu bar of
*Targets / Operations / Manage*, and three toolbars. *Upload Operation* and
*Manage Media Sets* are greyed until the ARINC 665 media set scan finishes.

Diagnostics go to `%TEMP%\arinc_615a_data_loader_gui.log` at `info` level — the
executable is linked `WIN32` and has no console.

Confirming the whole stack takes one FIND query: *Targets ▸ FIND Query*, accept
the broadcast address, *Commit*. After the receive timeout (3 s by default)
*Finish* enables. Closing the wizard refreshes the main window counters, and the
FIND TX table reads:

```
Packet Type            Packet Count   Packet Size
Information Request    1              4
```

One 4-byte FIND Information Request actually transmitted, no responses.

---

## 8. Failure modes

| Symptom | Cause | Fix |
| --- | --- | --- |
| `CMake 4.3 or higher is required` | `vcvars64.bat` put CMake 3.31 first on `PATH` | Set `CMAKE_EXE`, or use `scripts\configure-gui.bat` |
| vcpkg starts building 84 packages unexpectedly | `vcvars64.bat` overrode `VCPKG_ROOT` | Set `VCPKG_ROOT` before calling, or use the scripts |
| `Could NOT find Qt6` | `QT6_DIR` unset or wrong | Point it at the directory containing `lib/cmake/Qt6` |
| Application exits immediately, no window | Qt platform plugin missing | Run `scripts\deploy-qt.bat` |
| Window opens, every icon blank | `qsvg` image-format plugin missing | Run `scripts\deploy-qt.bat`; do not pass `--no-plugins` |
| `The code execution cannot proceed because boost_...dll was not found` | vcpkg DLLs not on `PATH` | Use `scripts\run-gui.bat` |
| `error C2220 ... C4127` in Boost headers | Warnings-as-errors re-enabled | Leave `CMAKE_COMPILE_WARNING_AS_ERROR` at `False` |
| `CMAKE_OBJECT_PATH_MAX` warnings for `arinc_665_media_set_*_gui` | Object paths 215–219 chars against a 250 limit | Warnings only; those targets are not in the build set. Move the checkout nearer the drive root to build them. |

---

## 9. Regenerating the documentation

`docs/CODE-TRACE.md` is generated from the HTML in `docs/code-trace-html/`:

```bash
pip install beautifulsoup4
python docs/render-code-trace.py
```

Edit the HTML, re-render, commit both. A hand-edit to the Markdown will be
overwritten.

---

## 10. Links

- [Qt 6.8 documentation](https://doc.qt.io/qt-6/)
- [aqtinstall](https://github.com/miurahr/aqtinstall)
- [windeployqt](https://doc.qt.io/qt-6/windows-deployment.html)
- [vcpkg](https://vcpkg.io/)
- [CMake presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)
- [Upstream ARINC 615A Tool Suite](https://git.thomas-vogt.de/thomas-vogt/arinc_615a)
- [CLI sibling repository](https://github.com/Hitheshkaranth/arinc-615a-cli-tool-suite)

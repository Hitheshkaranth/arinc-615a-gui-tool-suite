# Offline installation

Written from the position of someone who has to get this building on a machine
that will never see the internet — a bench in a lab, a certification
environment, a machine on an isolated network.

The short version: **the build has five network dependencies.** Remove all five
and it works air-gapped. Four of them are removed by carrying files across; the
fifth, the compiler, needs an offline installer layout.

---

## 1. What actually needs the network

| # | Dependency | Where it bites | Offline answer |
| --- | --- | --- | --- |
| 1 | **Six sibling projects** cloned by `FetchContent` from `git.thomas-vogt.de` | Configure | Vendor them in-tree — the project already has the hook |
| 2 | **~84 vcpkg packages** (Boost, libxml++, spdlog …) | Configure | Carry a populated `vcpkg_installed` tree, or the binary cache |
| 3 | **Qt 6.8.3** | Configure and run | Copy the Qt prefix; it is self-contained |
| 4 | **CMake 4.3.4** | Configure | Carry the official binary zip |
| 5 | **MSVC 2022 build tools** | Everything | Create an offline layout with `--layout` |

Nothing else reaches out. There is no telemetry call, no license check, no
package manager at run time.

> **Sizes.** Budget roughly **9–12 GB** for the full bundle: Qt ≈ 1.5 GB,
> vcpkg_installed ≈ 1.5 GB (debug + release), the VS offline layout ≈ 4–6 GB,
> the six sibling checkouts ≈ 250 MB, CMake ≈ 60 MB, this repository ≈ 40 MB.
> A 16 GB stick is comfortable.

---

## 2. Phase A — on a connected machine

Do all of this once, on a machine with internet, ideally the same Windows
version and the same Visual Studio version as the target.

### A1. Clone this repository

```bat
git clone https://github.com/Hitheshkaranth/arinc_615a_gui-tool-suite.git
cd arinc_615a_gui-tool-suite
```

Use a **full** clone, not `--depth 1`. The target machine has no remote to fetch
from later, and a shallow clone cannot be deepened offline.

### A2. Vendor the six sibling projects

This is the one that catches people out. `CMakeLists.txt` clones `helper`,
`qt_icon_resources`, `arinc-649`, `arinc_665`, `tftp` and `commands` at
configure time — but it prefers an in-tree checkout if one exists:

```cmake
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/helper )
  set( FETCHCONTENT_SOURCE_DIR_HELPER ${CMAKE_SOURCE_DIR}/helper )
endif()
```

Populate that hook:

```bat
scripts\fetch-deps.bat            REM clone or update all six
scripts\fetch-deps.bat --status   REM report what is vendored
```

`--status` is the line to look for before you carry anything across:

```
  [x] helper               ff446b3
  [x] qt_icon_resources    b5819588
  [x] arinc-649            fe1822c
  [x] arinc_665            ff787bd
  [x] tftp                 f1bb5fc
  [x] commands             74c6998

  All 6 vendored in-tree - configure works OFFLINE.
```

> **The directory names are not arbitrary.** CMake looks for exactly these, and
> `arinc-649` is **hyphenated** while every other name uses underscores. A
> directory called `arinc_649` is silently ignored and configure will try to
> clone.

The six directories are git-ignored — they are a local cache, not repository
content, so they will not be committed by accident.

### A3. Install Qt

```bat
python -m pip install aqtinstall
python -m aqt install-qt windows desktop 6.8.3 win64_msvc2022_64 -O C:\Qt
```

Do **not** pass `-m qtsvg`; for 6.8.3 it fails, and Qt SVG is already in the base
package. The whole `C:\Qt\6.8.3\msvc2022_64` tree gets carried across as-is — it
is relocatable, nothing in it is registered with the OS.

### A4. Get CMake 4.3.4

Visual Studio ships 3.31, which is too old — every `CMakeLists.txt` in this tree
declares `cmake_minimum_required( VERSION 4.3 )`. Download the official zip
(not the MSI, which needs an installer):

<https://github.com/Kitware/CMake/releases/download/v4.3.4/cmake-4.3.4-windows-x86_64.zip>

Unpack it somewhere in the bundle. It runs from any directory.

### A5. Do one full build

This is what populates `vcpkg_installed`, and it is the step that takes hours on
a cold machine — libiconv alone compiles for a long while. Do it here, not on
the target.

```bat
build.bat --no-run
```

Build the Release configuration too if the target needs it:

```bat
scripts\release-build.bat
```

### A6. Create the Visual Studio offline layout

From an existing VS installation's bootstrapper, or download
`vs_BuildTools.exe` fresh:

```bat
vs_BuildTools.exe --layout C:\vslayout ^
  --add Microsoft.VisualStudio.Workload.VCTools ^
  --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
  --add Microsoft.VisualStudio.Component.Windows11SDK.22621 ^
  --includeRecommended --lang en-US
```

Skip this step if the target machine already has VS 2022 with the C++ workload.

### A7. Stage the bundle

```
offline-bundle\
├── arinc_615a_gui-tool-suite\      the repository, including the six vendored dirs
├── Qt\6.8.3\msvc2022_64\           from A3
├── cmake-4.3.4-windows-x86_64\     from A4
├── vcpkg_installed\                copied from the build tree, see below
├── vcpkg\                          a vcpkg checkout (for its toolchain file only)
└── vslayout\                       from A6, omit if VS is already installed
```

`vcpkg_installed` comes out of the build directory:

```bat
robocopy cmake-build-msvc-static-debug-gui\vcpkg_installed ^
         offline-bundle\vcpkg_installed /E
```

> Do **not** carry `cmake-build-*` across wholesale. Those trees contain absolute
> paths baked into `CMakeCache.txt`, `build.ninja` and every `.obj`. They will
> not relocate, and a stale cache produces confusing failures. Carry
> `vcpkg_installed` only.

### A8. Record checksums

On the connected machine:

```powershell
Get-ChildItem offline-bundle -Recurse -File |
  Get-FileHash -Algorithm SHA256 |
  Export-Csv offline-bundle\MANIFEST.csv -NoTypeInformation
```

Verify on the target before you build anything. On an isolated network this is
usually a requirement rather than a nicety.

---

## 3. Phase B — on the air-gapped machine

### B1. Install the toolchain

If VS is not present, run `vslayout\vs_setup.exe` and select the same components
listed in A6. Reboot if it asks.

### B2. Unpack the bundle

Somewhere short. Path length matters — CMake warns above 250 characters for
object paths, and this project has deep directory names:

```
C:\a615\arinc_615a_gui-tool-suite\
C:\a615\Qt\6.8.3\msvc2022_64\
C:\a615\cmake-4.3.4-windows-x86_64\
C:\a615\vcpkg\
```

`C:\a615` rather than `C:\Users\<name>\Documents\Projects\...` is a deliberate
choice, not laziness.

### B3. Verify the manifest

```powershell
Import-Csv MANIFEST.csv | ForEach-Object {
  $actual = (Get-FileHash $_.Path -Algorithm SHA256).Hash
  if ($actual -ne $_.Hash) { Write-Error "MISMATCH: $($_.Path)" }
}
```

### B4. Point the environment at the bundle

```bat
set "CMAKE_EXE=C:\a615\cmake-4.3.4-windows-x86_64\bin\cmake.exe"
set "VCPKG_ROOT=C:\a615\vcpkg"
set "QT6_DIR=C:\a615\Qt\6.8.3\msvc2022_64"
```

These are the three variables `scripts\env-gui.bat` honours. Set them in the
shell, or as system environment variables so they survive a reboot.

### B5. Restore the vcpkg tree

```bat
cd C:\a615\arinc_615a_gui-tool-suite
robocopy C:\a615\offline-bundle\vcpkg_installed ^
         cmake-build-msvc-static-debug-gui\vcpkg_installed /E
```

The build directory does not exist yet; `robocopy` creates it. This is the
only part of a build tree that relocates safely.

### B6. Configure offline

```bat
scripts\offline-configure.bat
```

It refuses to start unless the preconditions hold, so a missing piece is
reported by name rather than as a CMake error four minutes later:

```
ERROR: sibling projects not vendored: arinc_665 tftp
       Run scripts\fetch-deps.bat on a connected machine first.
```

What it does differently from the normal configure is one flag:

```bat
cmake --preset msvc-static-debug-gui -DVCPKG_MANIFEST_INSTALL=OFF
```

`VCPKG_MANIFEST_INSTALL=OFF` stops vcpkg running an install during configure and
makes it use `vcpkg_installed` exactly as delivered. Without it, vcpkg checks
the manifest, decides something needs building, and reaches for the network.

### B7. Build, deploy, run

```bat
scripts\build-gui.bat
scripts\deploy-qt.bat
scripts\run-gui.bat
```

`deploy-qt.bat` copies from your local Qt prefix — it is a file copy, not a
download.

---

## 4. Verifying it is genuinely offline

Do not trust "it built" as proof; a machine with a working connection will build
whether or not the offline preparation was correct. Test the preparation on a
connected machine by making the dependency host unreachable:

```powershell
$env:GIT_CONFIG_COUNT   = "1"
$env:GIT_CONFIG_KEY_0   = "url.https://invalid.invalid/.insteadOf"
$env:GIT_CONFIG_VALUE_0 = "https://git.thomas-vogt.de/"
scripts\offline-configure.bat
```

Any attempted clone now fails loudly against `invalid.invalid` instead of
quietly succeeding. A clean configure under that redirect means the vendoring is
real.

Two things to grep the log for:

| Look for | Meaning if present |
| --- | --- |
| `Cloning into '...'` | A sibling project was **not** vendored — configure hit the network |
| `Running vcpkg install` | `VCPKG_MANIFEST_INSTALL=OFF` did not take effect |

Neither should appear.

---

## 5. What breaks, and why

| Symptom | Cause | Fix |
| --- | --- | --- |
| `Cloning into 'arinc_649-src'...` then a network error | Directory named `arinc_649`; CMake wants `arinc-649` | Rename it — the hyphen is load-bearing |
| `Could NOT find Qt6` | `QT6_DIR` unset, or points one level too high | It must contain `lib/cmake/Qt6` |
| `CMake 4.3 or higher is required … 3.31.6-msvc6` | `vcvars64.bat` put VS's CMake first on `PATH` | Set `CMAKE_EXE`; see [BUILD.md § 4](BUILD.md) |
| vcpkg starts building packages | `VCPKG_MANIFEST_INSTALL=OFF` missing, or `vcpkg_installed` not in place | Use `scripts\offline-configure.bat` |
| vcpkg rebuilds despite a populated tree | Compiler ABI differs from the preparing machine | Same VS version on both, or carry the binary cache instead |
| `CMAKE_OBJECT_PATH_MAX` warnings | Checkout path too deep | Unpack under a short root such as `C:\a615` |
| Application starts, every icon blank | `qsvg` image-format plugin missing | Re-run `scripts\deploy-qt.bat` |
| `boost_...dll was not found` | vcpkg DLLs not on `PATH` | Use `scripts\run-gui.bat` |

### The ABI caveat, stated plainly

`vcpkg_installed` is only reusable if the target machine's compiler produces a
matching ABI hash. Same VS 2022 with the same MSVC toolset version is fine; a
different toolset will make vcpkg decide the packages are stale and try to
rebuild them — offline, that fails.

If the two machines cannot be matched, carry `%LOCALAPPDATA%\vcpkg\archives`
(the binary cache) instead of `vcpkg_installed`, and set:

```bat
set "VCPKG_BINARY_SOURCES=clear;files,C:\a615\archives,read"
```

vcpkg then restores from the cache rather than downloading, and will rebuild
only what genuinely does not match — which may still fail offline if a source
tarball is needed. Matching the toolchains is the reliable route.

---

## 6. Air-gapped checklist

| On a connected machine | Carry across | Result |
| --- | --- | --- |
| `scripts\fetch-deps.bat` | the six sibling directories | configure needs no `git.thomas-vogt.de` |
| `aqt install-qt` | `Qt\6.8.3\msvc2022_64\` | configure and run need no Qt download |
| download the zip | `cmake-4.3.4-windows-x86_64\` | no CMake installer needed |
| one full build | `vcpkg_installed\` | no vcpkg download or compile |
| `vs_BuildTools --layout` | `vslayout\` | no Visual Studio download |
| — | `MANIFEST.csv` | integrity provable on arrival |

When all six rows are done, `scripts\offline-configure.bat` completes with no
network access at all.

---

## 7. Keeping it current

An air-gapped machine drifts. When you refresh:

1. `git pull` this repository **and** `scripts\fetch-deps.bat` on the connected
   machine — the sibling projects track `main` and move independently.
2. Rebuild on the connected machine so `vcpkg_installed` matches the new
   manifest.
3. Re-run the checksum step; carry the whole bundle again.

Carrying only the repository and reusing an old `vcpkg_installed` is the usual
way this goes wrong: the manifest changes, vcpkg notices, and it reaches for a
network that is not there.

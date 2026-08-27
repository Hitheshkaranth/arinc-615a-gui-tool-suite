@echo off
rem ============================================================================
rem  Configure with no network access.
rem
rem  Assumes the offline preparation has been done - see docs/OFFLINE-INSTALL.md:
rem    1. scripts\fetch-deps.bat has vendored the six sibling projects in-tree
rem    2. a populated vcpkg_installed tree has been copied into the build dir
rem    3. Qt is on disk and QT6_DIR points at it
rem
rem  VCPKG_MANIFEST_INSTALL=OFF is the switch that matters: it stops vcpkg from
rem  running an install during configure and makes it use vcpkg_installed as-is.
rem ============================================================================
setlocal EnableDelayedExpansion
set "GUI_ROOT=%~dp0..\"
cd /d "%~dp0.." || exit /b 1
call "%~dp0env-gui.bat" || exit /b 1

set "BUILD=%GUI_ROOT%cmake-build-msvc-static-debug-gui"

if not exist "%BUILD%\vcpkg_installed\x64-windows" (
  echo(
  echo ERROR: %BUILD%\vcpkg_installed\x64-windows is missing.
  echo        Copy it from the prepared bundle before configuring offline.
  echo        See docs\OFFLINE-INSTALL.md step 5.
  exit /b 1
)

set "MISSING="
for %%N in (helper qt_icon_resources arinc-649 arinc_665 tftp commands) do (
  if not exist "%GUI_ROOT%%%N" set "MISSING=!MISSING! %%N"
)
if not "%MISSING%"=="" (
  echo(
  echo ERROR: sibling projects not vendored:%MISSING%
  echo        Run scripts\fetch-deps.bat on a connected machine first.
  exit /b 1
)

echo Using cmake: %CMAKE_EXE%
echo Using Qt:    %QT6_DIR%
echo Offline:     vcpkg manifest install disabled, six siblings vendored
"%CMAKE_EXE%" --preset msvc-static-debug-gui -DVCPKG_MANIFEST_INSTALL=OFF %*

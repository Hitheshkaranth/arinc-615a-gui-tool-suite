@echo off
rem Shared environment setup for the ARINC 615A GUI build scripts.
rem Override any of CMAKE_EXE, VCPKG_ROOT, QT6_DIR before calling to use your own toolchain.
rem
rem Values set before this script runs win; note that vcvars64.bat injects its own VCPKG_ROOT
rem and prepends its own CMake 3.31 to PATH, so both are captured first and restored after.

set "_USER_VCPKG_ROOT=%VCPKG_ROOT%"

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1

if not "%_USER_VCPKG_ROOT%"=="" (
  set "VCPKG_ROOT=%_USER_VCPKG_ROOT%"
) else (
  set "VCPKG_ROOT=%GUI_ROOT%..\arinc_615a-main\arinc_615a-main\.tools\vcpkg"
)

if "%QT6_DIR%"=="" set "QT6_DIR=%GUI_ROOT%..\Qt\6.8.3\msvc2022_64"

rem The project requires CMake 4.3; Visual Studio ships 3.31.
if "%CMAKE_EXE%"=="" set "CMAKE_EXE=%GUI_ROOT%..\arinc_615a-main\arinc_615a-main\.tools\cmake-4.3.4-windows-x86_64\bin\cmake.exe"
if not exist "%CMAKE_EXE%" set "CMAKE_EXE=cmake"

exit /b 0

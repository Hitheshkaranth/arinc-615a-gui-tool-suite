@echo off
rem Configure the ARINC 615A graphical data loader.
setlocal
set "GUI_ROOT=%~dp0..\"
cd /d "%~dp0.." || exit /b 1
call "%~dp0env-gui.bat" || exit /b 1
echo Using cmake: %CMAKE_EXE%
echo Using vcpkg: %VCPKG_ROOT%
echo Using Qt:    %QT6_DIR%
"%CMAKE_EXE%" --preset msvc-static-debug-gui %*

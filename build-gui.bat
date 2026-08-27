@echo off
rem Build the ARINC 615A graphical data loader.
setlocal
set "GUI_ROOT=%~dp0"
cd /d "%~dp0" || exit /b 1
call "%GUI_ROOT%env-gui.bat" || exit /b 1
"%CMAKE_EXE%" --build --preset msvc-static-debug-gui %*

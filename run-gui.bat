@echo off
rem Launch the ARINC 615A Data Loader from the build tree.
rem Run windeployqt against the executable once before the first launch.
setlocal
set "ROOT=%~dp0"
cd /d "%~dp0" || exit /b 1
set "BUILD=%ROOT%cmake-build-msvc-static-debug-gui"
set "APPDIR=%BUILD%\app\arinc_615a_data_loader_gui"
rem vcpkg runtime DLLs (boost, libxml++, spdlog) are not copied next to the exe
set "PATH=%BUILD%\vcpkg_installed\x64-windows\debug\bin;%PATH%"
start "" "%APPDIR%\arinc_615a_data_loader_gui.exe" %*

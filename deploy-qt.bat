@echo off
rem Copy the Qt runtime and plugins next to the built executable.
setlocal
set "GUI_ROOT=%~dp0"
cd /d "%~dp0" || exit /b 1
call "%GUI_ROOT%env-gui.bat" || exit /b 1
"%QT6_DIR%\bin\windeployqt.exe" --debug --no-translations --no-opengl-sw ^
  "%GUI_ROOT%cmake-build-msvc-static-debug-gui\app\arinc_615a_data_loader_gui\arinc_615a_data_loader_gui.exe"

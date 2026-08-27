@echo off
rem Configure and build the Release configuration, then deploy the release Qt runtime.
setlocal
set "GUI_ROOT=%~dp0..\"
cd /d "%~dp0.." || exit /b 1
call "%~dp0env-gui.bat" || exit /b 1
"%CMAKE_EXE%" --preset msvc-static-release-gui || exit /b 1
"%CMAKE_EXE%" --build --preset msvc-static-release-gui || exit /b 1
"%QT6_DIR%\bin\windeployqt.exe" --release --no-translations --no-opengl-sw ^
  "%GUI_ROOT%cmake-build-msvc-static-release-gui\app\arinc_615a_data_loader_gui\arinc_615a_data_loader_gui.exe" || exit /b 1
echo RELEASE BUILD OK

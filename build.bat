@echo off
setlocal
rem ============================================================================
rem  ARINC 615A Data Loader (GUI) - one command: configure, build, deploy, run.
rem
rem    build.bat                 configure, build, deploy Qt, launch the GUI
rem    build.bat --no-run        configure, build and deploy only
rem    build.bat --no-deploy     skip windeployqt (already deployed once)
rem
rem  Qt must be installed first - see docs\BUILD.md. Point QT6_DIR at it if it
rem  is not at ..\Qt\6.8.3\msvc2022_64.
rem ============================================================================

set "REPO=%~dp0"
set "RUN=1"
set "DEPLOY=1"

:args
if "%~1"=="" goto ready
if /i "%~1"=="--no-run"    ( set "RUN=0"    & shift & goto args )
if /i "%~1"=="--no-deploy" ( set "DEPLOY=0" & shift & goto args )
shift
goto args
:ready

echo(
echo ==============================================================
echo  ARINC 615A Data Loader (GUI)
echo  configure  -^>  build  -^>  deploy Qt  -^>  run
echo ==============================================================

call "%REPO%scripts\configure-gui.bat" || (
  echo(
  echo Configure failed. See docs\BUILD.md.
  exit /b 1
)

call "%REPO%scripts\build-gui.bat" || (
  echo(
  echo Build failed. See docs\BUILD.md.
  exit /b 1
)

set "EXE=%REPO%cmake-build-msvc-static-debug-gui\app\arinc_615a_data_loader_gui\arinc_615a_data_loader_gui.exe"
if not exist "%EXE%" (
  echo ERROR: build reported success but %EXE% is missing.
  exit /b 1
)

if "%DEPLOY%"=="1" (
  call "%REPO%scripts\deploy-qt.bat" || (
    echo(
    echo windeployqt failed. Without it the GUI will not start.
    exit /b 1
  )
)

if "%RUN%"=="0" (
  echo(
  echo Built: %EXE%
  exit /b 0
)

echo(
echo ==============================================================
echo  Launching
echo ==============================================================
call "%REPO%scripts\run-gui.bat"

echo(
echo --------------------------------------------------------------
echo  Executable: %EXE%
echo  Launch again with the vcpkg DLLs on PATH:
echo    scripts\run-gui.bat
echo --------------------------------------------------------------
exit /b 0

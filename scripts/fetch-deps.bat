@echo off
rem ============================================================================
rem  Vendor the six sibling projects in-tree so configure never touches the
rem  network. Run this ONCE on a connected machine.
rem
rem    scripts\fetch-deps.bat            clone or update all six
rem    scripts\fetch-deps.bat --status   report what is vendored
rem
rem  The directory names are not arbitrary - CMakeLists.txt looks for exactly
rem  these and sets FETCHCONTENT_SOURCE_DIR_<NAME> when it finds one. Note that
rem  arinc-649 is hyphenated while every other name uses underscores.
rem ============================================================================
setlocal EnableDelayedExpansion
set "ROOT=%~dp0.."
cd /d "%ROOT%" || exit /b 1

set "BASE=https://git.thomas-vogt.de/thomas-vogt"
rem  dir name = repository name, except arinc-649
set "NAMES=helper qt_icon_resources arinc-649 arinc_665 tftp commands"

if /i "%~1"=="--status" goto status

for %%N in (%NAMES%) do (
  if exist "%%N\.git" (
    echo [update] %%N
    git -C "%%N" pull --ff-only || exit /b 1
  ) else (
    echo [clone ] %%N
    git clone --depth 1 "%BASE%/%%N.git" "%%N" || exit /b 1
  )
)

echo(
echo All six vendored. Configure will now use the in-tree checkouts.
goto status

:status
echo(
echo  Vendored dependency sources
echo  ---------------------------
set /a HAVE=0
for %%N in (%NAMES%) do (
  if exist "%%N\.git" (
    for /f "delims=" %%H in ('git -C "%%N" rev-parse --short HEAD') do echo   [x] %%-20N %%H
    set /a HAVE+=1
  ) else (
    echo   [ ] %%-20N MISSING
  )
)
echo(
if "!HAVE!"=="6" (
  echo  All 6 vendored in-tree - configure works OFFLINE.
) else (
  echo  !HAVE! of 6 vendored - configure still needs git.thomas-vogt.de.
)
exit /b 0

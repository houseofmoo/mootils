@echo off
setlocal
set PRESET=%~1
if "%PRESET%"=="" set PRESET=win-msvc-debug

set EXE=build\%PRESET%\bin\app.exe
if not exist "%EXE%" (
    echo Could not find "%EXE%"
    echo Build first, or pass a preset name: run.bat win-gnu-debug
    endlocal
    exit /b 1
)

"%EXE%"
endlocal
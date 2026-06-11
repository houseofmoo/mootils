@echo off
@REM setlocal

@REM set SEARCH_DIR=build\win-debug
@REM pushd "%SEARCH_DIR%" || ( echo Failed to enter "%SEARCH_DIR%" & exit /b 1 )

@REM rem Find the first .exe (alphabetically) in this directory
@REM for /f "usebackq delims=" %%F in (`dir /b /a:-d /o:n *.exe 2^>nul`) do (
@REM     set "FIRST_EXE=%%~fF"
@REM     goto :found
@REM )

@REM echo No .exe files found in "%SEARCH_DIR%".
@REM popd
@REM exit /b 1

@REM :found
@REM echo Launching "%FIRST_EXE%"...
@REM "%FIRST_EXE%"
@REM set "RC=%ERRORLEVEL%"
@REM popd
@REM exit /b %RC%


@REM start "" "build/win-debug/bin/app.exe"
start "" "build/win-msvc-debug/bin/Debug/app.exe"
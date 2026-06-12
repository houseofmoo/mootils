@echo off
setlocal

rem ============================================================
rem Usage:
rem   build.bat [--debug-gnu | --debug-msvc | --release-gnu | --release-msvc] [--clean] [--run]
rem ------------------------------------------------------------
rem Examples:
rem   build.bat --debug-gnu
rem   build.bat --release-gnu --clean
rem ------------------------------------------------------------
rem Manual cmake build commands:
rem     cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
rem     cmake --build build --parallel
rem ============================================================

set PRESET=win-msvc-debug
set BUILD_DIR=build
set CLEAN=false
set RUN_AFTER_BUILD=false

:parse_args
    if "%~1"=="" goto after_args

    if /I "%~1"=="--debug-gnu" (
        set PRESET=win-gnu-debug
    ) else if /I "%~1"=="--release-gnu" (
        set PRESET=win-gnu-release
    ) else if /I "%~1"=="--debug-msvc" (
        set PRESET=win-msvc-debug
    ) else if /I "%~1"=="--release-msvc" (
        set PRESET=win-msvc-release
    ) else if /I "%~1"=="--clean" (
        set CLEAN=true
    ) else if /I "%~1"=="--run" (
        @REM set RUN_AFTER_BUILD=true
        echo.
        echo Warning: --run option is disabled for libraries, ignoring
    ) else (
        echo Unknown option: %~1
        echo Usage: builder.bat [--debug-gnu | --debug-msvc | --release-gnu | --release-msvc] [--clean] [--run]
        endlocal
        exit /b 1
    )
    shift
    goto parse_args
:after_args

rem optional clean
if "%CLEAN%"=="true" (
    if exist "%BUILD_DIR%" (
        echo.
        echo Cleaning build directory "%BUILD_DIR%"...
        rmdir /s /q "%BUILD_DIR%"
        if errorlevel 1 (
            echo Failed to remove build directory
            endlocal
            exit /b 1
        )
    ) else (
        echo.
        echo Build directory "%BUILD_DIR%" does not exist, skipping clean
    )
)

echo.
echo Running CMake configure preset "%PRESET%"
cmake --preset %PRESET%
if errorlevel 1 (
    echo.
    echo CMake configuration failed
    endlocal
    exit /b 1
)

echo.
echo Building project for preset "%PRESET%"
cmake --build --preset %PRESET% --parallel
if errorlevel 1 (
    echo.
    echo Build failed
    endlocal
    exit /b 1
)

echo.
echo Build completed successfully for preset "%PRESET%"

@REM if "%RUN_AFTER_BUILD%"=="true" (
@REM     run.bat
@REM )

endlocal
exit /b 0
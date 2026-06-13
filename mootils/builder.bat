@echo off
setlocal

rem ============================================================
rem Usage:
rem   builder.bat [--debug | --release] [--gnu | --msvc] [--static | --shared] [--clean] [--run]
rem ------------------------------------------------------------
rem Examples:
rem   builder.bat --debug -gnu
rem   builder.bat --release -msvc --clean
rem ------------------------------------------------------------
rem Manual cmake build commands:
rem     cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
rem     cmake --build build --parallel
rem ============================================================

set BUILD_DIR=build
set PRESET=win

set MSVC=false
set RELEASE=false
set SHARED=false
set CLEAN=false
set RUN=false

:parse_args
    if "%~1"=="" goto after_args

    if /I "%~1"=="--debug" (
        set RELEASE=false
    ) else if /I "%~1"=="--release" (
        set RELEASE=true
    ) else if /I "%~1"=="--gnu" (
        set MSVC=false
    ) else if /I "%~1"=="--msvc" (
        set MSVC=true
    ) else if /I "%~1"=="--static" (
        set SHARED=false
    ) else if /I "%~1"=="--shared" (
        set SHARED=true
    ) else if /I "%~1"=="--clean" (
        set CLEAN=true
    ) else if /I "%~1"=="--run" (
        @REM set RUN=true
        echo.
        echo Warning: --run option is disabled for libraries, ignoring
    ) else (
        echo Unknown option: %~1
        echo Usage: builder.bat [--debug | --release] [--gnu | --msvc]  [--static | --shared] [--clean] [--run]
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

rem select build preset
if "%MSVC%"=="true" (
    set PRESET=%PRESET%-msvc
) else (
    set PRESET=%PRESET%-gnu
)
if "%RELEASE%"=="true" (
    set PRESET=%PRESET%-release
) else (
    set PRESET=%PRESET%-debug
)
if "%SHARED%"=="true" (
    set PRESET=%PRESET%-shared
) else (
    set PRESET=%PRESET%-static
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
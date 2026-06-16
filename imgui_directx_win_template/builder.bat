@echo off
setlocal

rem Usage:
rem   builder.bat [--debug | --release] [--gnu | --msvc] [--dx9 | --dx10 | --dx11 | --dx12] [--clean] [--run]
rem Examples:
rem   builder.bat --debug --msvc --dx11 --run
rem   builder.bat --release --gnu --dx9 --clean

set BUILD_DIR=build
set PRESET=win
set MSVC=false
set RELEASE=false
set CLEAN=false
set RUN=false
set DX_VERSION=11

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
    ) else if /I "%~1"=="--dx9" (
        set DX_VERSION=9
    ) else if /I "%~1"=="--dx10" (
        set DX_VERSION=10
    ) else if /I "%~1"=="--dx11" (
        set DX_VERSION=11
    ) else if /I "%~1"=="--dx12" (
        set DX_VERSION=12
    ) else if /I "%~1"=="--clean" (
        set CLEAN=true
    ) else if /I "%~1"=="--run" (
        set RUN=true
    ) else (
        echo Unknown option: %~1
        echo Usage: builder.bat [--debug ^| --release] [--gnu ^| --msvc] [--dx9 ^| --dx10 ^| --dx11 ^| --dx12] [--clean] [--run]
        endlocal
        exit /b 1
    )
    shift
    goto parse_args
:after_args

if "%CLEAN%"=="true" (
    if exist "%BUILD_DIR%" (
        echo Cleaning build directory "%BUILD_DIR%"...
        rmdir /s /q "%BUILD_DIR%"
        if errorlevel 1 (
            echo Failed to remove build directory
            endlocal
            exit /b 1
        )
    )
)

if "%MSVC%"=="true" (set PRESET=%PRESET%-msvc) else (set PRESET=%PRESET%-gnu)
if "%RELEASE%"=="true" (set PRESET=%PRESET%-release) else (set PRESET=%PRESET%-debug)

echo.
echo Configuring preset "%PRESET%" with DirectX %DX_VERSION%
cmake --preset %PRESET% -DIMGUI_DIRECTX_VERSION=%DX_VERSION%
if errorlevel 1 (
    echo CMake configuration failed
    endlocal
    exit /b 1
)

echo.
echo Building preset "%PRESET%"
cmake --build --preset %PRESET% --parallel
if errorlevel 1 (
    echo Build failed
    endlocal
    exit /b 1
)

echo.
echo Build completed successfully for preset "%PRESET%" using DirectX %DX_VERSION%

if "%RUN%"=="true" (
    call run.bat %PRESET%
)

endlocal
exit /b 0

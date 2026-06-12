cmake_minimum_required(VERSION 3.25)
project(glfw VERSION 3.4.0 LANGUAGES C)

if (NOT WIN32)
    message(FATAL_ERROR "This bundled GLFW copy has been trimmed for Windows x64 only.")
endif()

add_library(glfw STATIC)

set_target_properties(glfw PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME glfw3
)

target_sources(glfw PRIVATE
    src/context.c
    src/egl_context.c
    src/init.c
    src/input.c
    src/mappings.h
    src/monitor.c
    src/null_init.c
    src/null_joystick.c
    src/null_joystick.h
    src/null_monitor.c
    src/null_platform.h
    src/null_window.c
    src/osmesa_context.c
    src/platform.c
    src/platform.h
    src/vulkan.c
    src/wgl_context.c
    src/win32_init.c
    src/win32_joystick.c
    src/win32_joystick.h
    src/win32_module.c
    src/win32_monitor.c
    src/win32_platform.h
    src/win32_thread.c
    src/win32_thread.h
    src/win32_time.c
    src/win32_time.h
    src/win32_window.c
    src/window.c
    include/GLFW/glfw3.h
    include/GLFW/glfw3native.h
)

target_include_directories(glfw
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(glfw PRIVATE
    _GLFW_WIN32
    UNICODE
    _UNICODE
)

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    target_compile_definitions(glfw PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if (MINGW)
    target_compile_definitions(glfw PRIVATE WINVER=0x0A00)
endif()

# Needed by static GLFW consumers, especially with MinGW-style toolchains.
target_link_libraries(glfw PUBLIC
    gdi32
    user32
    shell32
)

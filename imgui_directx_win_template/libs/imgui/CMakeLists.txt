cmake_minimum_required(VERSION 3.25)
project(imgui LANGUAGES CXX)

set(IMGUI_BACKEND_CPP "${CMAKE_CURRENT_SOURCE_DIR}/src/backends/imgui_impl_dx${IMGUI_DIRECTX_VERSION}.cpp")
set(IMGUI_BACKEND_H   "${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends/imgui_impl_dx${IMGUI_DIRECTX_VERSION}.h")
set(IMGUI_WIN32_CPP   "${CMAKE_CURRENT_SOURCE_DIR}/src/backends/imgui_impl_win32.cpp")
set(IMGUI_WIN32_H     "${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends/imgui_impl_win32.h")

foreach(required_file IN LISTS IMGUI_BACKEND_CPP IMGUI_BACKEND_H IMGUI_WIN32_CPP IMGUI_WIN32_H)
    if(NOT EXISTS "${required_file}")
        message(FATAL_ERROR
            "Missing official Dear ImGui backend file:\n"
            "  ${required_file}\n\n"
            "This zip includes your existing ImGui core, but your original project did not vendor the Win32/DirectX backends.\n"
            "Run scripts/fetch_imgui_backends.ps1 from the project root, or copy these files from the same Dear ImGui version you are using."
        )
    endif()
endforeach()

add_library(imgui STATIC)

target_sources(imgui PRIVATE
    src/imgui.cpp
    src/imgui_widgets.cpp
    src/imgui_draw.cpp
    src/imgui_demo.cpp
    src/imgui_stdlib.cpp
    src/imgui_tables.cpp
    src/backends/imgui_impl_win32.cpp
    src/backends/imgui_impl_dx${IMGUI_DIRECTX_VERSION}.cpp
)

target_include_directories(imgui
    PRIVATE
        src
        src/backends
    PUBLIC
        include
)

target_compile_definitions(imgui PUBLIC
    IMGUI_DIRECTX_VERSION=${IMGUI_DIRECTX_VERSION}
    IMGUI_USE_DX${IMGUI_DIRECTX_VERSION}=1
)

if(MSVC)
    target_compile_options(imgui PRIVATE /W4 /permissive-)
else()
    target_compile_options(imgui PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(IMGUI_DIRECTX_VERSION STREQUAL "9")
    target_link_libraries(imgui PUBLIC d3d9)
elseif(IMGUI_DIRECTX_VERSION STREQUAL "10")
    target_link_libraries(imgui PUBLIC d3d10 dxgi d3dcompiler)
elseif(IMGUI_DIRECTX_VERSION STREQUAL "11")
    target_link_libraries(imgui PUBLIC d3d11 dxgi d3dcompiler)
elseif(IMGUI_DIRECTX_VERSION STREQUAL "12")
    target_link_libraries(imgui PUBLIC d3d12 dxgi d3dcompiler)
endif()

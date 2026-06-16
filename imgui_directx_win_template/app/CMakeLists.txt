cmake_minimum_required(VERSION 3.25)
project(app LANGUAGES CXX)

add_executable(app) # WIN32)

set_target_properties(app PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_sources(app PRIVATE
    src/main.cpp
    src/context/ui_context.cpp
)

target_include_directories(app PRIVATE src)

target_compile_definitions(app PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
    IMGUI_DIRECTX_VERSION=${IMGUI_DIRECTX_VERSION}
    IMGUI_USE_DX${IMGUI_DIRECTX_VERSION}=1
)

if(MSVC)
    target_compile_options(app PRIVATE
        /W4 /WX /permissive- /Zc:__cplusplus /Zc:preprocessor /EHsc
    )
else()
    target_compile_options(app PRIVATE
        -Wall -Wextra -Wpedantic -Werror
        -Wfloat-conversion -Wdouble-promotion -Wshadow -Wformat=2 -Wundef
        -Wnon-virtual-dtor -Woverloaded-virtual -Wnull-dereference
        -Wimplicit-fallthrough -Wcast-align -Wconversion
        # -Wsign-conversion
    )
endif()

target_link_libraries(app PRIVATE imgui user32 gdi32 imm32 dwmapi)

if(IMGUI_DIRECTX_VERSION STREQUAL "9")
    target_link_libraries(app PRIVATE d3d9)
elseif(IMGUI_DIRECTX_VERSION STREQUAL "10")
    target_link_libraries(app PRIVATE d3d10 dxgi d3dcompiler)
elseif(IMGUI_DIRECTX_VERSION STREQUAL "11")
    target_link_libraries(app PRIVATE d3d11 dxgi d3dcompiler)
elseif(IMGUI_DIRECTX_VERSION STREQUAL "12")
    target_link_libraries(app PRIVATE d3d12 dxgi d3dcompiler)
endif()

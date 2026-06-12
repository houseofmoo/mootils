cmake_minimum_required(VERSION 3.25)
project(glfw LANGUAGES C)

add_library(glfw STATIC)
add_library(glfw::glfw ALIAS glfw)

set(GLFW_COMMON_SOURCES
    include/GLFW/glfw3.h
    include/GLFW/glfw3native.h
    src/internal.h
    src/platform.h
    src/mappings.h
    src/context.c
    src/init.c
    src/input.c
    src/monitor.c
    src/platform.c
    src/vulkan.c
    src/window.c
    src/egl_context.c
    src/osmesa_context.c
    src/null_platform.h
    src/null_joystick.h
    src/null_init.c
    src/null_monitor.c
    src/null_window.c
    src/null_joystick.c
)

target_sources(glfw PRIVATE ${GLFW_COMMON_SOURCES})

target_include_directories(glfw
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(glfw PROPERTIES
    OUTPUT_NAME glfw3
    C_STANDARD 99
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE ON
)

if (WIN32)
    target_compile_definitions(glfw PRIVATE _GLFW_WIN32)
    target_sources(glfw PRIVATE
        src/win32_platform.h
        src/win32_joystick.h
        src/win32_thread.h
        src/win32_time.h
        src/win32_init.c
        src/win32_joystick.c
        src/win32_module.c
        src/win32_monitor.c
        src/win32_thread.c
        src/win32_time.c
        src/win32_window.c
        src/wgl_context.c
    )
    target_link_libraries(glfw PUBLIC gdi32)
elseif (UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    find_package(X11 REQUIRED)

    if (NOT X11_Xrandr_INCLUDE_PATH OR NOT X11_Xrandr_LIB)
        message(FATAL_ERROR "GLFW X11 support requires XRandR development files, e.g. libxrandr-dev.")
    endif()
    if (NOT X11_Xinerama_INCLUDE_PATH OR NOT X11_Xinerama_LIB)
        message(FATAL_ERROR "GLFW X11 support requires Xinerama development files, e.g. libxinerama-dev.")
    endif()
    if (NOT X11_Xcursor_INCLUDE_PATH OR NOT X11_Xcursor_LIB)
        message(FATAL_ERROR "GLFW X11 support requires Xcursor development files, e.g. libxcursor-dev.")
    endif()
    if (NOT X11_Xi_INCLUDE_PATH OR NOT X11_Xi_LIB)
        message(FATAL_ERROR "GLFW X11 support requires XInput development files, e.g. libxi-dev.")
    endif()
    if (NOT X11_Xext_INCLUDE_PATH OR NOT X11_Xext_LIB)
        message(FATAL_ERROR "GLFW X11 support requires Xext development files, e.g. libxext-dev.")
    endif()
    if (NOT X11_Xkb_INCLUDE_PATH)
        message(FATAL_ERROR "GLFW X11 support requires XKB headers, e.g. libx11-dev.")
    endif()
    if (NOT X11_Xshape_INCLUDE_PATH)
        message(FATAL_ERROR "GLFW X11 support requires X Shape headers, e.g. libxext-dev.")
    endif()

    target_compile_definitions(glfw PRIVATE _GLFW_X11)
    target_sources(glfw PRIVATE
        src/posix_module.c
        src/posix_poll.c
        src/posix_poll.h
        src/posix_thread.c
        src/posix_thread.h
        src/posix_time.c
        src/posix_time.h
        src/linux_joystick.c
        src/linux_joystick.h
        src/x11_init.c
        src/x11_monitor.c
        src/x11_platform.h
        src/x11_window.c
        src/xkb_unicode.c
        src/xkb_unicode.h
        src/glx_context.c
    )

    target_include_directories(glfw PRIVATE
        ${X11_INCLUDE_DIR}
        ${X11_Xrandr_INCLUDE_PATH}
        ${X11_Xinerama_INCLUDE_PATH}
        ${X11_Xcursor_INCLUDE_PATH}
        ${X11_Xi_INCLUDE_PATH}
        ${X11_Xext_INCLUDE_PATH}
        ${X11_Xkb_INCLUDE_PATH}
        ${X11_Xshape_INCLUDE_PATH}
    )
    target_link_libraries(glfw
        PUBLIC
            ${X11_X11_LIB}
            ${X11_Xrandr_LIB}
            ${X11_Xinerama_LIB}
            ${X11_Xcursor_LIB}
            ${X11_Xi_LIB}
            ${X11_Xext_LIB}
            Threads::Threads
            ${CMAKE_DL_LIBS}
            m
    )
else()
    message(FATAL_ERROR "This trimmed GLFW copy supports only Windows and Linux/X11.")
endif()

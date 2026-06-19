cmake_minimum_required(VERSION 3.25)
project(mootils_lib LANGUAGES C CXX)

# set language standards globally
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS OFF)

# direct output to builds/bin and builds/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# build as library
if (MOOTILS_SHARED_LIB)
    message(STATUS "Building as shared library")
    add_library(${PROJECT_NAME} SHARED)
    add_library(util::mootils ALIAS ${PROJECT_NAME})
else()
    message(STATUS "Building as static library")
    add_library(${PROJECT_NAME} STATIC)
    add_library(util::mootils ALIAS ${PROJECT_NAME})
endif()


# require language standards for ourselves and consumers of this library
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20 c_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# what compiler we using
message(STATUS "")
message(STATUS "==== Compiler Information ====")
message(STATUS "  C++ compiler      : ${CMAKE_CXX_COMPILER}")
message(STATUS "  C++ standard      : ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler ID       : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Compiler version  : ${CMAKE_CXX_COMPILER_VERSION}")
#message(STATUS "  Compile features  : ${CMAKE_CXX_COMPILE_FEATURES}")
message(STATUS "")
message(STATUS "  C compiler        : ${CMAKE_C_COMPILER}")
message(STATUS "  C standard        : ${CMAKE_C_STANDARD}")
message(STATUS "  Compiler ID       : ${CMAKE_C_COMPILER_ID}")
message(STATUS "  Compiler version  : ${CMAKE_C_COMPILER_VERSION}")
#message(STATUS "  Compile features  : ${CMAKE_C_COMPILE_FEATURES}")
message(STATUS "==============================")
message(STATUS "")

# parse log level from arguments, error if not set
set(MOO_LOG_LEVEL "Debug" CACHE STRING "Set log level (Debug, Info, Warning, Error, None)")
set_property(CACHE MOO_LOG_LEVEL PROPERTY STRINGS Debug Info Warning Error None)
if(MOO_LOG_LEVEL STREQUAL "Debug")
    message(STATUS "Log level set to DEBUG")
    set(MOO_LOG_LEVEL_VALUE 0)
elseif(MOO_LOG_LEVEL STREQUAL "Info")
    message(STATUS "Log level set to INFO")
    set(MOO_LOG_LEVEL_VALUE 1)
elseif(MOO_LOG_LEVEL STREQUAL "Warning")
    message(STATUS "Log level set to WARNING")
    set(MOO_LOG_LEVEL_VALUE 2)
elseif(MOO_LOG_LEVEL STREQUAL "Error")
    message(STATUS "Log level set to ERROR")
    set(MOO_LOG_LEVEL_VALUE 3)
elseif(MOO_LOG_LEVEL STREQUAL "None")
    message(STATUS "Log level set to NONE")
    set(MOO_LOG_LEVEL_VALUE 99)
else()
    message(STATUS "Log level set to WARNING")
    set(MOO_LOG_LEVEL_VALUE 2)
endif()

# macro defs
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        $<$<PLATFORM_ID:Windows>:MOO_WIN32>
        $<$<PLATFORM_ID:Linux>:MOO_LINUX>
        MOO_LOGQ_LOG_LEVEL=${MOO_LOG_LEVEL_VALUE}
        MOOTILS_AS_SHARED_LIB=$<BOOL:${MOOTILS_SHARED_LIB}>
    PRIVATE
        MOOTILS_LIB_BUILD
)

# source files
target_sources(${PROJECT_NAME}
    PRIVATE
        #platform independent sources
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mem/arena.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mem/byte_buffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mem/cyclic_byte_buffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/seri/byte_reader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/seri/byte_writer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shm/shm.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/str/str_utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/print/log_q.cpp

        # windows only
        $<$<PLATFORM_ID:Windows>:
            ${CMAKE_CURRENT_SOURCE_DIR}/src/evt/win/win_named_semaphore.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/evt/win/win_semaphore.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/win/win_platform.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/shm/win/win_shm.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/win/win_map_err.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/win/win_socket_context.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/win/win_tcp_client.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/win/win_tcp_server.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/win/win_tcp_socket.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/win/win_udp_multicast.cpp
        >

        # linux only
        $<$<PLATFORM_ID:Linux>:
            ${CMAKE_CURRENT_SOURCE_DIR}/src/evt/linux/linux_named_semaphore.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/evt/linux/linux_semaphore.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/linux/linux_platform.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/shm/linux/linux_shm.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/linux/linux_map_err.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/linux/linux_socket_context.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/linux/linux_tcp_client.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/linux/linux_tcp_server.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/linux/linux_tcp_socket.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sock/linux/linux_udp_multicast.cpp
        >
)

# local headers
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# extensive but reasonable warnings
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:
    $<$<CXX_COMPILER_ID:MSVC>:
      /W4 /WX /permissive-
      /w14242 /w14254 /w14263 /w14265 /w14287
      /w14545 /w14546 /w14547 /w14549
    >
    $<$<CXX_COMPILER_ID:Clang>:
      -Wall -Wextra -Wpedantic -Werror
      -Wfloat-conversion
      -Wold-style-cast
      -Wdouble-promotion
      -Wshadow
      -Wformat=2
      -Wundef
      -Wnon-virtual-dtor
      -Woverloaded-virtual
      -Wnull-dereference
      -Wimplicit-fallthrough
      -Wsign-conversion
      -Wcast-align
      -Wconversion
    >
    $<$<CXX_COMPILER_ID:GNU>:
      -Wall -Wextra -Wpedantic -Werror
      -Wsign-conversion
      -Wfloat-conversion
      -Wold-style-cast
      -Wdouble-promotion
      -Wshadow
      -Wformat=2
      -Wundef
      -Wnon-virtual-dtor
      -Woverloaded-virtual
      -Wnull-dereference
      -Wimplicit-fallthrough
      -Wsign-conversion
      -Wcast-align
      -Wduplicated-cond
      -Wduplicated-branches
      -Wlogical-op
      -Wmisleading-indentation
      -Wconversion
    >
  >
)

# external system libs
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        $<$<PLATFORM_ID:Windows>:ws2_32>
        $<$<PLATFORM_ID:Linux>:pthread>
)
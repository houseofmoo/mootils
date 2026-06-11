cmake_minimum_required(VERSION 3.15)
project(mootils LANGUAGES C CXX)

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

# direct output to builds/bin and builds/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# include subdirectories
add_subdirectory(lib)
add_subdirectory(tests)
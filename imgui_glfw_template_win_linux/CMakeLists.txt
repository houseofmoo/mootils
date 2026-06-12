cmake_minimum_required(VERSION 3.25)
project(imgui_win_linux_template LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw-3.4)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app)

# copy assets to output directory
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         "${CMAKE_SOURCE_DIR}/assets"
#         "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
#     COMMENT "Copying assets directory"
# )
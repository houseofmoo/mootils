cmake_minimum_required(VERSION 3.25)
project(app LANGUAGES C CXX)

add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/context/ui_context.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src/
)

target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:
    $<$<CXX_COMPILER_ID:GNU>:
      -Wall -Wextra -Wpedantic -Werror
      -Wfloat-conversion
      # -Wold-style-cast
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

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        imgui
)
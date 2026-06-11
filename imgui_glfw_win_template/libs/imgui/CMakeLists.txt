cmake_minimum_required(VERSION 3.15)
project(imgui LANGUAGES C CXX)

# cpp version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# c version
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} 
    PRIVATE
        src/imgui.cpp
        src/imgui_widgets.cpp
        src/imgui_draw.cpp
        src/imgui_demo.cpp
        src/imgui_tables.cpp
        src/backends/imgui_impl_glfw.cpp
        src/backends/imgui_impl_opengl3.cpp
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        src/
        src/backends
    PUBLIC
        include/
)

# add open GL
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        glfw
        OpenGL::GL
)
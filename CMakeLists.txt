cmake_minimum_required(VERSION 3.29)
project(Final VERSION 1.0.0)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_executable(Final
        src/config.h
        src/scene.cpp
        src/glad.c
        src/shaders/shader.cpp)

target_include_directories(Final
        PRIVATE external/glad/include
        PRIVATE external/glfw-3.4/include
        PRIVATE external/glm
        PRIVATE external/tinygltf
        PRIVATE external
        PRIVATE src)

target_link_directories(Final
        PRIVATE external/glfw-3.4/lib-mingw-w64
        PRIVATE external/glm
        PRIVATE src/shaders)

target_link_libraries(Final
        glfw3
        ${OPENGL_LIBRARY})
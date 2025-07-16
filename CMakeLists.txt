cmake_minimum_required(VERSION 3.31)
project(ppm-viewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/PPMImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/src/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/tinyfd/tinyfiledialogs.c
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/lib/libglfw3.a
)

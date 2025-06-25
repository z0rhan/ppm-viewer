cmake_minimum_required(VERSION 3.31)
project(ppm-viewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/parser.cpp
    ${CMAKE_SOURCE_DIR}/vendor/stb_image/stb_image_write.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/stb_image
)

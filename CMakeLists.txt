cmake_minimum_required(VERSION 3.20)

project(SoftwareRasterizer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2PP REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(SoftwareRasterizer
    "src/main.cpp"
    "src/Screen.cpp"
    "src/Screen.h"
)

target_include_directories(SoftwareRasterizer PRIVATE ${SDL2PP_INCLUDE_DIRS})

target_link_libraries(SoftwareRasterizer PRIVATE ${SDL2PP_LIBRARIES})
target_link_libraries(SoftwareRasterizer PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_link_libraries(SoftwareRasterizer PRIVATE spdlog::spdlog)
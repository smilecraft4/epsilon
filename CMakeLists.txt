cmake_minimum_required(VERSION 3.20)

project(Epsilon)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find_package(SDL2PP REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(epsilon
    "src/main.cpp"
    "src/Core/Colors.h"
    "src/Core/Vectors.h"
    "src/Core/Maths.h"
    "src/Window.h"
    "src/Window.cpp"
    "src/Mesh.h"
    "src/Rasterizer.h"
    "src/Rasterizer.cpp"
    "src/Core/Maths.cpp"
    "src/Texture.h"
)

target_precompile_headers(epsilon PRIVATE "src/pch.h")

# target_include_directories(epsilon PRIVATE ${SDL2PP_INCLUDE_DIRS})

# target_link_libraries(epsilon PRIVATE ${SDL2PP_LIBRARIES})
target_link_libraries(epsilon PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_link_libraries(epsilon PRIVATE spdlog::spdlog)
target_link_libraries(epsilon PRIVATE glm::glm)
target_link_libraries(epsilon PRIVATE glad::glad)
target_link_libraries(epsilon PRIVATE glfw)

ADD_CUSTOM_TARGET(link_target ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)
cmake_minimum_required(VERSION 3.20)

project(Epsilon)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2PP REQUIRED)
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
  "src/Screen.h"
  "src/Screen.cpp"
  "src/Rasterizer.h"
  "src/Rasterizer.cpp"
 "src/Core/Maths.cpp")

target_include_directories(epsilon PRIVATE ${SDL2PP_INCLUDE_DIRS})

target_link_libraries(epsilon PRIVATE ${SDL2PP_LIBRARIES})
target_link_libraries(epsilon PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_link_libraries(epsilon PRIVATE spdlog::spdlog)
target_link_libraries(epsilon PRIVATE glm::glm)
target_link_libraries(epsilon PRIVATE glad::glad)
target_link_libraries(epsilon PRIVATE glfw)
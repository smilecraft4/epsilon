#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "Vectors.h"

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct Pixel {};

class Canvas {
  public:
    Canvas(int width, int height);
    ~Canvas();

    void SetPixel(Vector2Int pos, Color color);
    Color GetPixel(Vector2Int pos);

  private:
    int width_;
    int height_;

    std::vector<Color> pixels_;
};

class Window {
  public:
    Window(int width, int height, std::string name) {}
    ~Window() {}

  private:
    int width_;
    int height_;
    std::string name;

    GLFWwindow *window;

    std::unique_ptr<Canvas> canvas_;
};

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "Rasterizer.h"
#include "Texture.h"

class Window {
  public:
    Window(int width, int height, std::string name);
    ~Window() = default;

    void SetTitle(const std::string &title);

    bool Running() const;
    void Render(float delta_time);

    void RenderTexture(const Texture<Color> *texture);
    void Cleanup();

    static void PollEvents();
    static void Init(int argc, char **argv);
    static void Terminate();

  protected:
    virtual void OnRender(float delta_time) = 0;
    virtual void OnResizeEvent(int width, int height) = 0;
    virtual void OnMouseMoveEvent(float x, float y) = 0;
    virtual void OnKeyEvent(int key, int action) = 0;
    virtual void OnMouseButtonEvent(int button, int action) = 0;

    int width_;
    int height_;
    std::string name_;

  private:

    std::shared_ptr<Rasterizer> rasterizer_;

    GLFWwindow *glfw_window_;
    static void GlfwKeyCallback(GLFWwindow *glfw_window, int key, int scancode, int action, int mods);
    static void GlfwDropCallback(GLFWwindow *glfw_window, int path_count, const char *paths[]);
    static void GlfwScrollCallback(GLFWwindow *glfw_window, double xoffset, double yoffset);
    static void GlfwCursorPosCallback(GLFWwindow *glfw_window, double xpos, double ypos);
    static void GlfwCursorEnterCallback(GLFWwindow *glfw_window, int entered);
    static void GlfwWindowPosCallback(GLFWwindow *glfw_window, int xpos, int ypos);
    static void GlfwWindowSizeCallback(GLFWwindow *glfw_window, int width, int height);
    static void GlfwMouseButtonCallback(GLFWwindow *glfw_window, int button, int action, int mods);
    static void GlfwWindowCloseCallback(GLFWwindow *glfw_window);
    static void GlfwWindowFocusCallback(GLFWwindow *glfw_window, int focused);
    static void GlfwWindowRefreshCallback(GLFWwindow *glfw_window);
    static void GlfwFramebufferSizeCallback(GLFWwindow *glfw_window, int width, int height);

    static void GlfwErrorCallback(int error_code, const char *description);

    GLuint texture_sampling_mode_ = GL_NEAREST;

    GLuint screen_vao_;
    GLuint screen_program_;
    GLuint screen_texture_;
};


#include <glad/glad.h>
#include <spdlog/spdlog.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Screen.h"
#include "Window.h"

static void GlfwErrorCallback(int error_code, const char *description) {
    spdlog::error("{}: {}", error_code, description);
}

int main(int argc, char *argv[]) {
    spdlog::info("{}", argv[0]);

    if (!glfwInit()) {
        spdlog::critical("Failed to initialiaze GLFW");
        return -1;
    }

    glfwSetErrorCallback(GlfwErrorCallback);

    auto window = std::make_unique<Window>(800, 600, "Epsilon - Software Rasterizer");
    auto screen = std::make_unique<Screen>(256, 224, 0xFF00FFFF);

    glfwSwapInterval(1);
    double last_time = 0, current_time = 0, delta_time = 0.0;
    while (window->Running()) {
        last_time = current_time;
        current_time = glfwGetTime();
        delta_time = current_time - last_time;

        screen->RandomFill();

        window->RenderScreen(screen.get());
        window->SetTitle(
            std::format("Epsilon - Software Rasterizer | {:.1f}fps | {:.4f}ms", 1.0 / delta_time, delta_time));

        Window::PollEvents();
    }

    window->Cleanup();

    glfwTerminate();

    return 0;
}
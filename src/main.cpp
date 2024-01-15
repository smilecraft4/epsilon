#include <format>

#include <spdlog/spdlog.h>

#include "Core/Maths.h"
#include "Rasterizer.h"
#include "Texture.h"
#include "Window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

static Color RandomColor() { return Color(rand() % 255, rand() % 255, rand() % 255); }

class EpsilonWindow final : public Window {
  public:
    EpsilonWindow() : Window(800, 600, "Epsilon Rasterizer") {
        rasterizer_ = std::make_unique<Rasterizer>(800 / 4, 600 / 4);

        cursor_ = {.position = {-0.5f, 0.5f, 0.0f}, .color = {255, 51, 23}};
        b_ = {.position = {0.0f, 0.0f, 0.0f}, .color = {0, 51, 255}};

        drawing_ = false;
        line_ = std::vector<Vertex>();
        line_.reserve(4096);
        line_color_ = RandomColor();
    }
    ~EpsilonWindow() { Window::~Window(); }

  protected:
    void OnRender(float delta_time) override {

        if (drawing_) {
            drawing_ = true;

            Vertex point{};
            point.position = cursor_.position;
            // point.color = Color(((a_.position.x + 1.0) / 2.0) * 255, ((a_.position.y + 1.0) / 2.0) * 255, 0);
            point.color = line_color_;
            line_.push_back(point);
        }

        rasterizer_->Clear();

        rasterizer_->DrawVertex(cursor_);
        rasterizer_->DrawVertex(b_);

        for (size_t i = 0; i < line_.size(); i++) {
            rasterizer_->DrawVertex(line_[i]);
        }

        RenderTexture(rasterizer_->ColorBuffer());
    }
    void OnResizeEvent(int width, int height) override {}

    void OnMouseMoveEvent(float x, float y) override {
        cursor_.position.x = x / (width_ - 1.0) * 2.0f - 1.0f;
        cursor_.position.y = (y / (height_ - 1.0) * 2.0f - 1.0f) * -1;

    }
    void OnMouseButtonEvent(int button, int action) override {
        if (button == GLFW_MOUSE_BUTTON_1) {
            if (action == GLFW_PRESS) {
                drawing_ = true;
                line_color_ = RandomColor();
            } else if (action == GLFW_RELEASE) {
                drawing_ = false;
            }
        }
    }

    void OnKeyEvent(int key, int action) override {}

  private:
    std::vector<Vertex> line_;

    Color line_color_;

    Vertex cursor_;
    Vertex b_;
    std::unique_ptr<Rasterizer> rasterizer_;

    bool drawing_;
};

int main(int argc, char *argv[]) {
    Window::Init(argc, argv);

    EpsilonWindow window;

    double last_time = 0;
    double current_time = 0;
    double delta_time = 0.0;
    while (window.Running()) {
        last_time = current_time;
        current_time = glfwGetTime();
        delta_time = current_time - last_time;
        window.SetTitle(std::format("Epsilon | {:.1f}fps | {:.4f}ms", 1.0 / delta_time, delta_time));
        Window::PollEvents();
        window.Render(delta_time);
    }

    window.Cleanup();
    Window::Terminate();
    return 0;
}
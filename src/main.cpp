#include <format>

#include <spdlog/spdlog.h>

#include "Core/Maths.h"
#include "Rasterizer.h"
#include "Texture.h"
#include "Window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

static Color RandomColor() {
    Color c{};
    c.r = rand() % 255;
    c.g = rand() % 255;
    c.b = rand() % 255;
    // c.a = rand() % 255;
    return c;
}

static float RandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

static Vector3 RandomPosition(Vector3 min, Vector3 max) {
    Vector3 p{};
    p.x = RandomFloat(min.x, max.x);
    p.y = RandomFloat(min.y, max.y);
    p.z = RandomFloat(min.z, max.z);
    return p;
}

static void RandomVertices(int count, std::vector<Vertex> *vertices) {
    vertices->clear();
    srand(glfwGetTime() * 1000);
    for (size_t i = 0; i < (size_t)(count * 2); i++) {
        Vertex a{};

        a.color = RandomColor();
        a.position = RandomPosition({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});

        vertices->push_back(a);
    }
}

static void DrawPairOfLines(Rasterizer *rasterizer, const std::vector<Vertex> &vertices, float total_time, int count) {
    count = std::min((int)vertices.size(), count);
    for (size_t i = 0; i < floor(count / 2); i++) {
        srand(i);
        float offset = RandomFloat(0.0f, 100.0f) * 10.0f;

        float time = (cos(total_time + offset) + 1.0f) * 0.5f; // goes from 0 to 1

        Vertex c = Vertex::Lerp(vertices[i * 2 + 0], vertices[i * 2 + 1], time);

        rasterizer->DrawLine(vertices[i * 2 + 0], vertices[i * 2 + 1]);
        rasterizer->DrawVertex(c);
    }
}

// static Color RandomColor() { return Color(rand() % 255, rand() % 255, rand() % 255); }

class EpsilonWindow final : public Window {
  public:
    EpsilonWindow() : Window(800, 600, "Epsilon Rasterizer") {
        rasterizer_ = std::make_unique<Rasterizer>(256, 224);

        cursor_ = {.position = {0.0f, 0.0f, 0.0f}, .color = {255, 255, 255}};

        draw_lines_ = true;
        draw_lines_horizontal_ = true;
        draw_lines_vertical_ = true;
        draw_lines_cursor_ = true;
        draw_lines_angled = true;

        constructing_line_ = false;
        preview_vertex_ = cursor_;
        preview_closed_loop_ = false;
        line_color_ = RandomColor();
        vertices_ = std::vector<std::vector<Vertex>>();
    }
    ~EpsilonWindow() { Window::~Window(); }

  protected:
    void OnRender(float delta_time) override {
        rasterizer_->Clear();

        for (size_t i = 0; i < vertices_.size(); i++) {
            for (size_t j = 0; j < vertices_[i].size() - 1; j++) {
                rasterizer_->DrawLine(vertices_[i][j + 0], vertices_[i][j + 1]);
            }
            for (size_t j = 0; j < vertices_[i].size(); j++) {
                Vertex handle = vertices_[i][j];
                handle.color = Color(255, 0, 0);
                rasterizer_->DrawVertex(handle);
            }
        }
        if (constructing_line_) {
            Vertex next_point = preview_vertex_;
            next_point.color = preview_closed_loop_ ? Color(255, 255, 255) : line_color_;
            int index = vertices_.size() - 1;
            rasterizer_->DrawLine(vertices_[index][vertices_[index].size() - 1], next_point);
        }

        /*
        Vertex a{.position = {-0.6f, 0.0f, 0.0f}, .color = {128, 0, 0}};
        Vertex b{.position = {0.7f, 0.0f, 0.0f}, .color = {128, 0, 128}};
        Vertex c{.position = {-0.6f, 0.75f, 0.0f}, .color = {0, 128, 0}};
        Vertex d{.position = {0.7f, 0.75f, 0.0f}, .color = {128, 128, 0}};

        Vertex e{.position = {-0.8f, 0.1f, 0.0f}, .color = {0, 128, 0}};
        Vertex f{.position = {-0.8f, -0.9f, 0.0f}, .color = {128, 128, 0}};

        if (draw_lines_) {
            if (draw_lines_horizontal_) {
                rasterizer_->DrawLine(a, b);
                rasterizer_->DrawLine(c, d);

                rasterizer_->DrawLine(e, f);
            }
            if (draw_lines_vertical_) {
                rasterizer_->DrawLine(a, c);
                rasterizer_->DrawLine(b, d);
            }

            if (draw_lines_angled) {
                rasterizer_->DrawLine(a, d);
                rasterizer_->DrawLine(b, c);
            }

            if (draw_lines_cursor_) {
                rasterizer_->DrawLine(a, cursor_);
                rasterizer_->DrawLine(b, cursor_);
                rasterizer_->DrawLine(c, cursor_);
                rasterizer_->DrawLine(d, cursor_);
            }
        }


        rasterizer_->DrawVertex(a);
        rasterizer_->DrawVertex(b);
        rasterizer_->DrawVertex(c);
        rasterizer_->DrawVertex(d);
        */

        rasterizer_->DrawVertex(cursor_);

        RenderTexture(rasterizer_->ColorBuffer());
    }

    void OnResizeEvent(int width, int height) override {}
    void OnMouseScrollEvent(float x, float y) override {}

    void OnMouseMoveEvent(float x, float y) override {
        cursor_.position.x = x / width_ * 2.0f - 1.0f;
        cursor_.position.y = (y / height_ * 2.0f - 1.0f) * -1;

        preview_vertex_ = cursor_;

        if (constructing_line_) {
            preview_vertex_ = cursor_;

            int index = vertices_.size() - 1;
            int last_index = vertices_[index].size() - 1;

            const auto a = rasterizer_->ViewportToScreenspace(cursor_.position);
            const auto b = rasterizer_->ViewportToScreenspace(vertices_[index][0].position);

            if (Vector3::Distance(a, b) < 5.0f) {
                preview_vertex_ = vertices_[index][0];

                for (size_t i = 0; i < vertices_[index].size(); i++) {
                    vertices_[index][i].color = Color(255, 255, 255);
                }
                preview_closed_loop_ = true;
            } else if (preview_closed_loop_) {
                for (size_t i = 0; i < vertices_[index].size(); i++) {
                    vertices_[index][i].color = line_color_;
                }
                preview_closed_loop_ = false;
            }
        }
    }

    void OnMouseButtonEvent(int button, int action) override {
        if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
            // add a new point
            Vertex point{};
            point.position = cursor_.position;
            point.color = line_color_;

            if (!constructing_line_) {
                constructing_line_ = true;
                vertices_.push_back(std::vector<Vertex>(1, point));
            } else {
                if (preview_closed_loop_) {
                    point = preview_vertex_;
                    point.color = Color(255, 255, 255);
                    vertices_[vertices_.size() - 1].push_back(point);
                    constructing_line_ = false;
                } else {
                    vertices_[vertices_.size() - 1].push_back(point);
                }
            }
        }

        if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
            constructing_line_ = false;

            line_color_ = RandomColor();
        }
    }

    void OnKeyEvent(int key, int action) override {
        if (key == GLFW_KEY_L && action == GLFW_PRESS) {
            draw_lines_ = !draw_lines_;
        }
        if (key == GLFW_KEY_H && action == GLFW_PRESS) {
            draw_lines_horizontal_ = !draw_lines_horizontal_;
        }
        if (key == GLFW_KEY_V && action == GLFW_PRESS) {
            draw_lines_vertical_ = !draw_lines_vertical_;
        }
        if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            draw_lines_cursor_ = !draw_lines_cursor_;
        }
        if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            draw_lines_angled = !draw_lines_angled;
        }
    }

  private:
    Vertex cursor_;

    std::unique_ptr<Rasterizer> rasterizer_;

    bool draw_lines_;
    bool draw_lines_horizontal_;
    bool draw_lines_vertical_;
    bool draw_lines_angled;
    bool draw_lines_cursor_;

    bool preview_closed_loop_;
    bool constructing_line_;
    Color line_color_;
    Vertex preview_vertex_;
    std::vector<std::vector<Vertex>> vertices_;
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
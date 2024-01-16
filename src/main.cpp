#include <format>

#include <spdlog/spdlog.h>

#include "Core/Maths.h"
#include "Mesh.h"
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
        spdlog::set_level(spdlog::level::info);

        rasterizer_ = std::make_unique<Rasterizer>(256, 224);

        cursor_ = {.position = {0.0f, 0.0f, 0.0f}, .color = {255, 255, 255}};

        rotate_model_ = true;

        draw_cube_ = false;
        draw_quad_ = false;
        draw_tri_ = true;

        cube_ = Model::LoadFromObj("./data/meshes/SM_Cube.obj");
        quad_ = Model::LoadFromObj("./data/meshes/SM_Quad.obj");
        tri_ = Model::LoadFromObj("./data/meshes/SM_Triangle.obj");
    }
    ~EpsilonWindow() { Window::~Window(); }

  protected:
    void OnRender(float delta_time) override {
        rasterizer_->Clear();

        if (rotate_model_) {
            for (size_t i = 0; i < cube_.meshes.size(); i++) {
                const Vector3 rotation_rate = Vector3(1.5f, 0.25f, 1.005f) * delta_time;
                for (size_t j = 0; j < cube_.meshes[i].vertices.size(); j++) {

                    //  Point of Rotation = (X1, Y1, Z1)
                    //  Point Location    = (X1+A, Y1+B, Z1+C)

                    const Vector3 pivot_point = Vector3(0.0, 0.0, 0.0);
                    const Vector3 original_pos = cube_.meshes[i].vertices[j].position;

                    //  (Point Location - Point of Rotation) = (A, B, C).
                    Vector3 offset_pos = original_pos - pivot_point;
                    Vector3 x_rot = offset_pos;
                    Vector3 y_rot = offset_pos;
                    Vector3 z_rot = offset_pos;

                    //  A' = A*cos ZAngle - B*sin ZAngle
                    //  B' = A*sin ZAngle + B*cos ZAngle
                    //  C' = C.
                    z_rot.x = offset_pos.x * cos(rotation_rate.z) - offset_pos.y * sin(rotation_rate.z);
                    z_rot.y = offset_pos.x * sin(rotation_rate.z) + offset_pos.y * cos(rotation_rate.z);
                    z_rot.z = offset_pos.z;

                    //  C'' = C'*cos YAngle - A'*sin YAngle
                    //  A'' = C'*sin YAngle + A'*cos YAngle
                    //  B'' = B'
                    y_rot.z = z_rot.z * cos(rotation_rate.y) - z_rot.x * sin(rotation_rate.y);
                    y_rot.x = z_rot.z * sin(rotation_rate.y) + z_rot.x * cos(rotation_rate.y);
                    y_rot.y = z_rot.y;

                    //  B''' = B''*cos XAngle - C''*sin XAngle
                    //  C''' = B''*sin XAngle + C''*cos XAngle
                    //  A''' = A''
                    x_rot.y = y_rot.y * cos(rotation_rate.x) - y_rot.z * sin(rotation_rate.x);
                    x_rot.z = y_rot.y * sin(rotation_rate.x) + y_rot.z * cos(rotation_rate.x);
                    x_rot.x = y_rot.x;

                    // Rotated Point = (X1+A''', Y1+B''', Z1+C''');
                    cube_.meshes[i].vertices[j].position = pivot_point + x_rot;
                }
            }
        }

        if (draw_tri_) {
            for (size_t i = 0; i < tri_.meshes.size(); i++) {
                for (size_t j = 0; j < tri_.meshes[i].vertices.size() / 3; j++) {
                    rasterizer_->DrawLine(tri_.meshes[i].vertices[j * 3 + 0], tri_.meshes[i].vertices[j * 3 + 1]);
                    rasterizer_->DrawLine(tri_.meshes[i].vertices[j * 3 + 1], tri_.meshes[i].vertices[j * 3 + 2]);
                    rasterizer_->DrawLine(tri_.meshes[i].vertices[j * 3 + 2], tri_.meshes[i].vertices[j * 3 + 0]);
                }
            }
        }

        if (draw_quad_) {
            for (size_t i = 0; i < quad_.meshes.size(); i++) {
                for (size_t j = 0; j < quad_.meshes[i].vertices.size() / 3; j++) {
                    rasterizer_->DrawLine(quad_.meshes[i].vertices[j * 3 + 0], quad_.meshes[i].vertices[j * 3 + 1]);
                    rasterizer_->DrawLine(quad_.meshes[i].vertices[j * 3 + 1], quad_.meshes[i].vertices[j * 3 + 2]);
                    rasterizer_->DrawLine(quad_.meshes[i].vertices[j * 3 + 2], quad_.meshes[i].vertices[j * 3 + 0]);
                }
            }
        }

        if (draw_cube_) {
            for (size_t i = 0; i < cube_.meshes.size(); i++) {
                for (size_t j = 0; j < cube_.meshes[i].vertices.size() / 3; j++) {
                    rasterizer_->DrawLine(cube_.meshes[i].vertices[j * 3 + 0], cube_.meshes[i].vertices[j * 3 + 1]);
                    rasterizer_->DrawLine(cube_.meshes[i].vertices[j * 3 + 1], cube_.meshes[i].vertices[j * 3 + 2]);
                    rasterizer_->DrawLine(cube_.meshes[i].vertices[j * 3 + 2], cube_.meshes[i].vertices[j * 3 + 0]);
                }
            }

            for (size_t i = 0; i < cube_.meshes.size(); i++) {
                for (size_t j = 0; j < cube_.meshes[i].vertices.size() / 3; j++) {
                    rasterizer_->DrawLine(cube_.meshes[i].vertices[j * 3 + 0], cursor_);
                    rasterizer_->DrawLine(cube_.meshes[i].vertices[j * 3 + 1], cursor_);
                    rasterizer_->DrawLine(cube_.meshes[i].vertices[j * 3 + 2], cursor_);
                }
            }
        }

        rasterizer_->DrawVertex(cursor_);

        RenderTexture(rasterizer_->ColorBuffer());
    }

    void OnResizeEvent(int width, int height) override {}
    void OnMouseScrollEvent(float x, float y) override {
        rasterizer_->near_clip = std::min(-0.01f, rasterizer_->near_clip - (y * 0.05f));
        rasterizer_->far_clip = std::max(0.01f, rasterizer_->far_clip + (y * 0.05f));

        spdlog::info("near_clip = {}, far_clip = {}", rasterizer_->near_clip, rasterizer_->far_clip);
    }
    void OnMouseMoveEvent(float x, float y) override {
        cursor_.position.x = x / width_ * 2.0f - 1.0f;
        cursor_.position.y = (y / height_ * 2.0f - 1.0f) * -1;
    }
    void OnMouseButtonEvent(int button, int action) override {}
    void OnKeyEvent(int key, int action) override {
        if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
            draw_cube_ = !draw_cube_;
        }
        if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
            draw_tri_ = !draw_tri_;
        }
        if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
            draw_quad_ = !draw_quad_;
        }

        if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
            rotate_model_ = !rotate_model_;
        }
    }

  private:
    Vertex cursor_;

    bool rotate_model_;

    bool draw_cube_;
    bool draw_quad_;
    bool draw_tri_;

    Model cube_;
    Model quad_;
    Model tri_;

    std::unique_ptr<Rasterizer> rasterizer_;
};

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);
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
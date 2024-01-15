#include <format>

#include "Core/Maths.h"
#include "Rasterizer.h"
#include "Window.h"

#include "Texture.h"

static Color RandomColor() {
    Color c{};
    c.r = rand() % 255;
    c.g = rand() % 255;
    c.b = rand() % 255;
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

static void RandomVertices(int count, std::vector<Vertex> *vertices, std::vector<float> *offset) {
    vertices->clear();
    offset->clear();
    for (size_t i = 0; i < (size_t)(count * 2); i++) {
        offset->push_back(RandomFloat(0.0f, 100.0f) * 10.0f);

        for (size_t j = 0; j < 2; j++) {
            Vertex a{};
            a.color = RandomColor();
            a.position = RandomPosition({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0});
            vertices->push_back(a);
        }
    }
}

static uint32_t ColorToHex(Color color) { return *reinterpret_cast<uint32_t *>(&color); }

static void DrawPairOfLines(Texture<uint32_t> *texture, const std::vector<Vertex> &vertices,
                            const std::vector<float> &offset, float total_time) {
    for (size_t i = 0; i < vertices.size() / 2; i++) {
        float time = (cos(total_time + offset[i]) + 1.0f) * 0.5f; // goes from 0 to 1

        Vertex c = Vertex::Lerp(vertices[i * 2 + 0], vertices[i * 2 + 1], time);

        // Vector2Int a_pos{};
        // a_pos.x = (vertices[i * 2 + 0].position.x * texture->width);
        // a_pos.y = (vertices[i * 2 + 0].position.y * texture->height);
        // screen->SetPixel(a_pos, vertices[i * 2 + 0].color);

        // Vector2Int b_pos{};
        // b_pos.x = (vertices[i * 2 + 1].position.x * texture->width);
        // b_pos.y = (vertices[i * 2 + 1].position.y * texture->height);
        // screen->SetPixel(b_pos, vertices[i * 2 + 1].color);

        Vector2Int c_pos{};
        c_pos.x = (c.position.x * texture->width);
        c_pos.y = (c.position.y * texture->height);
        texture->SetPixel(c_pos.x, c_pos.y, ColorToHex(c.color));
    }
}

int main(int argc, char *argv[]) {
    Window::Init(argc, argv);

    Window window(800, 600, "Epsilon - Software Rasterizer");

    Texture<uint32_t> texture(800, 600);

    double last_time = 0, current_time = 0, delta_time = 0.0;
    while (window.Running()) {
        last_time = current_time;
        current_time = glfwGetTime();
        delta_time = current_time - last_time;
        window.SetTitle(std::format("Epsilon | {:.1f}fps | {:.4f}ms", 1.0 / delta_time, delta_time));
        Window::PollEvents();

        // Drawing happens here
        texture.Fill(0xFF000000u);


        // Presenting here
        window.RenderTexture(&texture);
    }

    window.Cleanup();

    Window::Terminate();

    return 0;
}
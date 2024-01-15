#include <format>

#include "Core/Maths.h"
#include "Rasterizer.h"
#include "Screen.h"
#include "Window.h"

static Color RandomColor() {
    Color c;
    c.r = rand() % 255;
    c.g = rand() % 255;
    c.b = rand() % 255;
    c.a = rand() % 255;
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
    for (size_t i = 0; i < (size_t)(count * 2); i++) {
        Vertex a{};

        a.color = RandomColor();
        a.position = RandomPosition({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0});

        vertices->push_back(a);
    }
}

static void DrawPairOfLines(Screen *screen, const std::vector<Vertex> &vertices, float total_time) {
    for (size_t i = 0; i < vertices.size() / 2; i++) {
        srand(i);
        float offset = RandomFloat(0.0f, 100.0f) * 10.0f;

        float time = (cos(total_time + offset) + 1.0f) * 0.5f; // goes from 0 to 1

        Vertex c = Vertex::Lerp(vertices[i * 2 + 0], vertices[i * 2 + 1], time);

        Vector2Int a_pos{};
        a_pos.x = floor(vertices[i * 2 + 0].position.x * screen->Width());
        a_pos.y = floor(vertices[i * 2 + 0].position.y * screen->Height());
        //screen->SetPixel(a_pos, vertices[i * 2 + 0].color);

        Vector2Int b_pos{};
        b_pos.x = floor(vertices[i * 2 + 1].position.x * screen->Width());
        b_pos.y = floor(vertices[i * 2 + 1].position.y * screen->Height());
        //screen->SetPixel(b_pos, vertices[i * 2 + 1].color);

        Vector2Int c_pos{};
        c_pos.x = floor(c.position.x * screen->Width());
        c_pos.y = floor(c.position.y * screen->Height());
        screen->SetPixel(c_pos, c.color);
    }
}

int main(int argc, char *argv[]) {
    Window::Init(argc, argv);

    auto window = std::make_unique<Window>(800, 600, "Epsilon - Software Rasterizer");
    auto screen = std::make_unique<Screen>(800, 600, 0x000000FF);

    std::vector<Vertex> vertices;
    RandomVertices(8192*4, &vertices);

    double last_time = 0, current_time = 0, delta_time = 0.0;
    while (window->Running()) {
        last_time = current_time;
        current_time = glfwGetTime();
        delta_time = current_time - last_time;

        screen->Clear(0x000000FF);

        DrawPairOfLines(screen.get(), vertices, current_time);

        window->RenderScreen(screen.get());
        window->SetTitle(
            std::format("Epsilon - Software Rasterizer | {:.1f}fps | {:.4f}ms", 1.0 / delta_time, delta_time));

        Window::PollEvents();
    }

    window->Cleanup();

    Window::Terminate();

    return 0;
}
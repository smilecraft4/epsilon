#include <format>

#include "Core/Maths.h"
#include "Rasterizer.h"
#include "Screen.h"
#include "Window.h"

int main(int argc, char *argv[]) {
    Window::Init(argc, argv);

    auto window = std::make_unique<Window>(800, 600, "Epsilon - Software Rasterizer");
    auto screen = std::make_unique<Screen>(60, 60, 0x000000FF);

    Vertex a{};
    a.position = {0.5f, 0.9f, 0.0f};
    a.color = 0xFF0000FF;

    Vertex b{};
    b.position = {0.1f, 0.7f, 0.8f};
    b.color = 0x0000FFFF;

    Vector2Int a_pos{};
    a_pos.x = floor(a.position.x * screen->Width());
    a_pos.y = floor(a.position.y * screen->Height());
    Vector2Int b_pos{};
    b_pos.x = floor(b.position.x * screen->Width());
    b_pos.y = floor(b.position.y * screen->Height());

    double last_time = 0, current_time = 0, delta_time = 0.0;
    while (window->Running()) {
        last_time = current_time;
        current_time = glfwGetTime();
        delta_time = current_time - last_time;

        // Draw a Line
        float time = (cos(current_time) + 1.0f) * 0.5f; // goes from 0 to 1

        Vertex c{};
        c.position = Vector3::Lerp(a.position, b.position, time);
        c.color = Color::Lerp(a.color, b.color, time);

        Vector2Int c_pos{};
        c_pos.x = floor(c.position.x * screen->Width());
        c_pos.y = floor(c.position.y * screen->Height());


        screen->Clear(0x000000FF);

        screen->SetPixel(a_pos, a.color);
        screen->SetPixel(b_pos, b.color);

        screen->SetPixel(c_pos, c.color);

        window->RenderScreen(screen.get());
        window->SetTitle(
            std::format("Epsilon - Software Rasterizer | {:.1f}fps | {:.4f}ms", 1.0 / delta_time, delta_time));

        Window::PollEvents();
    }

    window->Cleanup();

    Window::Terminate();

    return 0;
}
#include "Screen.h"
#include <SDL2/SDL.h>
#include <ctime>
#include <random>

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

float randomColor(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

Color HueToRGB(float hue) {
    Color rgb{0};

    int side = static_cast<int>(hue / 60);

    switch (side) {
    case 0: // red or yellow
        rgb.r = 255;
        rgb.g = 128;
        break;
    case 1: // yellow or green
        rgb.g = 255;
        rgb.r = 128;
        break;
    case 2: // green or cyan
        rgb.g = 255;
        rgb.b = 128;
        break;
    case 3: // cyan or blue
        rgb.b = 255;
        rgb.g = 128;
        break;
    case 4: // blue or magenta
        rgb.b = 255;
        rgb.r = 128;
        break;
    case 5: // magenta or red
        rgb.r = 255;
        rgb.b = 128;
        break;
    default: // red
        rgb.r = 255;
        break;
    }

    return rgb;
}

void generateRandomTriangles(int count, std::vector<Vector> *vertices, std::vector<Color> *colors) {
    vertices->clear();
    colors->clear();

    for (size_t i = 0; i < count; i++) {
        vertices->push_back(Vector{randomFloat(-1, 1), randomFloat(-1, 1), randomFloat(-255, 0)});
        vertices->push_back(Vector{randomFloat(-1, 1), randomFloat(-1, 1), randomFloat(-255, 0)});
        vertices->push_back(Vector{randomFloat(-1, 1), randomFloat(-1, 1), randomFloat(-255, 0)});

        colors->push_back(HueToRGB(randomFloat(0.0f, 360.0f)));
    }
}

void updateTriangles(std::vector<Vector> &vertices, float totalTime) {
    for (size_t i = 0; i < vertices.size(); i += 3) {
        vertices[i].x = cos(totalTime);
    }
}

int main(int argv, char **args) {
    srand(static_cast<unsigned>(time(0)));
    Screen screen(800, 600);

    auto start = Vector{randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f)};

    int triangle_count = 4;
    std::vector<Vector> vertices{};
    std::vector<Color> colors{};

    screen.on_mouse_wheel = [&triangle_count, &vertices, &colors](int value) {
        triangle_count = std::max(triangle_count + value, 0);

        generateRandomTriangles(triangle_count, &vertices, &colors);
    };

    generateRandomTriangles(triangle_count, &vertices, &colors);

    bool generated = false;
    float last_generation_time{0.0f};
    float spam_timer = 100.0f / 1'000.0f;

    float currentTime{0}, lastTime{0}, elapsedTime{0}, total_time{0};

    while (!screen.KeyDown(SDLK_ESCAPE)) {
        currentTime = SDL_GetTicks() / 1000.0f;
        elapsedTime = currentTime - lastTime;
        total_time += elapsedTime;

        if (screen.KeyDown(SDLK_b)) {
            if (generated == true) {
                if (last_generation_time + spam_timer < total_time) {
                    generated = false;
                }
            }

            if (!generated) {
                generateRandomTriangles(triangle_count, &vertices, &colors);
                last_generation_time = total_time;
                generated = true;
            }
        }

        updateTriangles(vertices, total_time);

        screen.ClearScreen();

        if (screen.KeyDown(SDLK_1)) {
            screen.show_depth_buffer = true;
        } else if (screen.show_depth_buffer) {
            screen.show_depth_buffer = false;
        }

        if (screen.KeyDown(SDLK_SPACE)) {
            for (size_t i = 0; i < triangle_count; i++) {
                size_t v = i * 3;
                screen.DrawWireTriangle(vertices[v + 0], vertices[v + 1], vertices[v + 2], colors[i]);
            }

        } else {
            for (size_t i = 0; i < triangle_count; i++) {
                size_t v = i * 3;
                screen.DrawTriangle(vertices[v + 0], vertices[v + 1], vertices[v + 2], colors[i]);
            }
        }

        screen.Render();
        screen.SetTitle(std::to_string(1 / elapsedTime));
        lastTime = currentTime;
    }

    return 0;
}

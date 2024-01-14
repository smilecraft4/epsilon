#include "Screen.h"

Screen::Screen(int width, int height, std::string title) : width(width), height(height), title(title) {
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                              SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    surface = nullptr;

    const auto size{width * height};
    pixels = std::vector<Color>(size, {0, 0, 0});
    depths = std::vector<Uint8>(size, 0);
}

Screen::~Screen() {}

void Screen::Quit() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Screen::ClearScreen(Color color) {
    std::fill(pixels.begin(), pixels.end(), color);
    std::fill(depths.begin(), depths.end(), 0);
}

void Screen::DrawWireTriangle(Vector v1, Vector v2, Vector v3, Color color) {
    auto s1 = MapToScreen(v1);
    auto s2 = MapToScreen(v2);
    auto s3 = MapToScreen(v3);

    PutLine(s1, s2, {255, 0, 0});
    PutLine(s2, s3, {0, 255, 0});
    PutLine(s3, s1, {0, 0, 255});
}

void Screen::DrawTriangle(Vector v1, Vector v2, Vector v3, Color color) {
    // TODO: unit testing
    // TODO: try with variable instead of vector for vertice & edges
    // TODO: calculate faces normals, calculate vertices normals
    std::vector<VectorInt> vertices = {MapToScreen(v1),  // will be highest
                                       MapToScreen(v2),  // will be midlle
                                       MapToScreen(v3)}; // will be lowest
    // Sort vector from highest .y to lowest .y
    if (vertices[0].y > vertices[1].y)
        std::swap(vertices[0], vertices[1]);
    if (vertices[0].y > vertices[2].y)
        std::swap(vertices[0], vertices[2]);
    if (vertices[1].y > vertices[2].y)
        std::swap(vertices[1], vertices[2]);

    std::vector<int> edges = {
        std::abs(vertices[0].y - vertices[2].y),  // line highest, second <- longest lenght
        std::abs(vertices[0].y - vertices[1].y),  // line second, last    <- closest lenght from highest vertices
        std::abs(vertices[1].y - vertices[2].y)}; // line highest, last   <- furthest lenght from highset vertices

    VectorInt a, b;
    for (size_t i = 0; i < edges[0]; i++) {
        // from top to bottom longest with closet
        if (i < edges[1]) {
            a = {Screen::Lerp(vertices[0].x, vertices[2].x, i, edges[0]),
                 Screen::Lerp(vertices[0].y, vertices[2].y, i, edges[0]),
                 Screen::Lerp(vertices[0].z, vertices[2].z, i, edges[0])};
            b = {Screen::Lerp(vertices[0].x, vertices[1].x, i, edges[1]),
                 Screen::Lerp(vertices[0].y, vertices[1].y, i, edges[1]),
                 Screen::Lerp(vertices[0].z, vertices[1].z, i, edges[1])};
            PutLine(a, b, color);
        }
        // continue longest and start furthest
        else {
            a = {Screen::Lerp(vertices[0].x, vertices[2].x, i, edges[0]),
                 Screen::Lerp(vertices[0].y, vertices[2].y, i, edges[0]),
                 Screen::Lerp(vertices[0].z, vertices[2].z, i, edges[0])};
            b = {Screen::Lerp(vertices[1].x, vertices[2].x, i - edges[1], edges[2]),
                 Screen::Lerp(vertices[1].y, vertices[2].y, i - edges[1], edges[2]),
                 Screen::Lerp(vertices[1].z, vertices[2].z, i - edges[1], edges[2])};
            PutLine(a, b, color);
        }
    }
}

void Screen::DrawLine(float x1, float y1, float x2, float y2, Color color) { DrawLine({x1, y1}, {x2, y2}, color); }
void Screen::DrawLine(Vector v1, Vector v2, Color color) {
    // convert from world space to screen space with aspect ratio
    VectorInt s1 = MapToScreen(v1);
    VectorInt s2 = MapToScreen(v2);
    VectorInt s;

    const int l_width = std::abs(s2.x - s1.x);
    const int l_height = std::abs(s2.y - s1.y);
    // calculate the slope to see if we go by rows or columns
    if (l_width <= l_height) {
        DrawStepped(s1, s2, l_height, color);
    } else {
        DrawStepped(s1, s2, l_width, color);
    }
}

void Screen::PutLine(VectorInt s1, VectorInt s2, Color color) {
    const int l_width = std::abs(s2.x - s1.x);
    const int l_height = std::abs(s2.y - s1.y);
    // calculate the slope to see if we go by rows or columns
    if (l_width <= l_height) {
        DrawStepped(s1, s2, l_height, color);
    } else {
        DrawStepped(s1, s2, l_width, color);
    }
}

void Screen::DrawStepped(VectorInt s1, VectorInt s2, int range, Color color) {
    VectorInt s;
    for (int p = 0; p < range; p++) {
        s.x = Lerp(s1.x, s2.x, p, range);
        s.y = Lerp(s1.y, s2.y, p, range);
        s.z = Lerp(s1.z, s2.z, p, range) * -1;

        PutPixel(s, color * (s.z / 255.0f));
    }
}

inline int Screen::Lerp(int a, int b, int p, int range) {
    if (a == b)
        return a;
    return p * (b - a) / range + a;
}

void Screen::DrawPixel(float x, float y, Color color) { DrawPixel({x, y}, color); }
void Screen::DrawPixel(Vector v, Color color) { PutPixel(MapToScreen(v), color); }

bool Screen::KeyDown(int keyCode) { return keys[keyCode]; }

void Screen::SetTitle(std::string newTitle) { title = newTitle; }

void Screen::Render() {
    ProcessInput();
    SDL_SetWindowTitle(window, title.c_str());

    if (show_depth_buffer) {
        for (size_t i = 0; i < pixels.size(); i++) {
            pixels[i] = Color{depths[i], depths[i], depths[i], 255};
        }
    }

    Uint32 *t_pixels;
    int t_pitch;
    SDL_LockTexture(texture, NULL, (void **)&t_pixels, &t_pitch);
    memcpy(t_pixels, pixels.data(), pixels.size() * 4);

    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void Screen::ProcessInput() {
    // TODO: add mouse support
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            keys[SDLK_ESCAPE] = true;
            break;
        case SDL_KEYDOWN:
            keys[event.key.keysym.sym] = true;
            break;
        case SDL_KEYUP:
            keys[event.key.keysym.sym] = false;
            break;
        case SDL_MOUSEWHEEL:
            if (event.wheel.y > 0) {
                on_mouse_wheel(1);
            } else if (event.wheel.y < 0) {
                on_mouse_wheel(-1);
            }
            break;
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                SDL_GetRendererOutputSize(renderer, &width, &height);
                texture =
                    SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
                const auto size{width * height};
                pixels.resize(size);
                depths.resize(size);
                ClearScreen();
                break;
            }
        }
    }
}

void Screen::PutPixel(VectorInt v, Color color) {
    if ((0 <= v.x && v.x < width) && (0 <= v.y && v.y < height)) // verify if pixel position is in bound
    {
        int index = v.x + v.y * width; // get the index position from screen space
        if (depths[index] < v.z) {
            pixels[index] = color; // set vector to new color
            depths[index] = v.z;
        }
    }
}

VectorInt Screen::MapToScreen(const Vector &v) {
    VectorInt s;
    const float w = width - 1;
    const float h = height - 1;

    float ratio = w / h;
    s.x = static_cast<int>(w / (2.0f * ratio) * v.x - w / (2.0f * ratio) * -ratio);
    s.y = static_cast<int>(h / 2.0f * v.y + h / 2.0f);

    // Setup clip_min & clip_max
    float clip_min = -255.0f;
    float clip_max = 0.0f;

    // z goes from 0 (closest) to -255 (futhest)
    s.z = static_cast<int>(std::min(std::max(v.z, clip_min), clip_max));

    return s;
}
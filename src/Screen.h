#include <SDL2/SDL.h>
#include <cmath>
#include <functional>
#include <map>
#include <string>
#include <vector>

struct Color {
    Uint8 r{0};
    Uint8 b{0};
    Uint8 g{0};
    Uint8 a{255};

    Color operator*(const float &b) {
        Color col;

        col.r = std::max(std::min(static_cast<int>(this->r * b), 255), 0);
        col.g = std::max(std::min(static_cast<int>(this->g * b), 255), 0);
        col.b = std::max(std::min(static_cast<int>(this->b * b), 255), 0);
        col.a = this->a;

        return col;
    }
};

struct Vector {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};

    Vector operator+(const Vector &b) {
        Vector vec;

        vec.x = this->x + b.x;
        vec.y = this->y + b.y;
        vec.z = this->z + b.z;

        return vec;
    }

    Vector operator*(const float &b) {
        Vector vec;

        vec.x = this->x * b;
        vec.y = this->y * b;
        vec.z = this->z * b;

        return vec;
    }
};
struct VectorInt {
    int x{0};
    int y{0};
    int z{0};
};

class Screen {
  public:
    Screen(int width, int height, std::string title = {"SDL2 Screen"});
    ~Screen();
    void Quit();
    void Render();

    void ClearScreen(Color color = {0, 0, 0});
    void DrawWireTriangle(Vector v1, Vector v2, Vector v3, Color color);
    void DrawTriangle(Vector v1, Vector v2, Vector v3, Color color);
    void DrawLine(float x1, float y1, float x2, float y2, Color color);
    void DrawLine(Vector v1, Vector v2, Color color);
    void DrawPixel(float x, float y, Color color);
    void DrawPixel(Vector v, Color color);
    bool KeyDown(int keyCode);
    void SetTitle(std::string newTitle);

    bool show_depth_buffer;
    std::function<void(int)> on_mouse_wheel;

  private:
    std::string title;
    int width;
    int height;

    std::vector<Color> pixels;
    std::vector<Uint8> depths;
    std::map<int, bool> keys;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Surface *surface;
    SDL_Event event;

  private:
    void ProcessInput();
    void PutLine(VectorInt s1, VectorInt s2, Color color);
    void PutPixel(VectorInt v, Color color);
    void DrawStepped(VectorInt s1, VectorInt s2, int step, Color color);
    inline int Lerp(int a, int b, int t, int max_t);
    VectorInt MapToScreen(const Vector &v);
};
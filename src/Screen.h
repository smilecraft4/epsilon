#pragma once

#include <vector>

#include "Core/Colors.h"
#include "Core/Vectors.h"

class Screen {
  public:
    Screen(int width, int height, Color color);
    ~Screen() = default;

    [[nodiscard]] std::vector<Color> Pixels() const { return pixels_; }
    [[nodiscard]] int Width() const { return width_; }
    [[nodiscard]] int Height() const { return height_; }

    void Resize(int width, int height);
    void SetPixel(int x, int y, Color color);
    void SetPixel(Vector2Int position, Color color);
    void RandomFill();
    void Clear(Color color);

    Vector2 Texel();

  private:
    int width_;
    int height_;
    Color background_color_;

    std::vector<Color> pixels_;
    Vector2 texel_;
};

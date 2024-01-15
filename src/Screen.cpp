#include "Screen.h"

Screen::Screen(int width, int height, Color color) : width_(width), height_(height), background_color_(color) {
    pixels_ = std::vector<Color>(width_ * height_, background_color_);

    texel_.x = 1.0 / width_;
    texel_.y = 1.0 / height_;
}

void Screen::Resize(int width, int height) {
    width_ = width;
    height_ = height;

    pixels_ = std::vector<Color>(width_ * height_, background_color_);

    texel_.x = 1.0 / width_;
    texel_.y = 1.0 / height_;
}

void Screen::SetPixel(int x, int y, Color color) { SetPixel(Vector2Int{x, y}, color); }
void Screen::SetPixel(Vector2Int pos, Color color) {
    if (pos.x >= 0 && pos.x < width_ && pos.y >= 0 && pos.y < height_) {
        size_t index = pos.x + pos.y * width_;
        pixels_[index] = color;
    }
}

void Screen::RandomFill() {
    for (size_t i = 0; i < width_ * height_; i++) {
        pixels_[i].r = (uint8_t)(rand() % 255);
        pixels_[i].g = (uint8_t)(rand() % 255);
        pixels_[i].b = (uint8_t)(rand() % 255);
    }
}

void Screen::Clear(Color color) { pixels_ = std::vector<Color>(width_ * height_, color); }

Vector2 Screen::Texel() { return texel_; }

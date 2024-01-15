#pragma once

#include <concepts>
#include <vector>

#include "Core/Colors.h"

template <typename T>
concept PixelFormat =
    std::is_same<T, Color>::value || std::is_same<T, uint16_t>::value || std::is_same<T, uint32_t>::value;

template <PixelFormat T> struct Texture {
    size_t width;
    size_t height;
    std::vector<T> pixels;

    void SetPixel(size_t x, size_t y, T pixel) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            size_t i = x + y * width;
            pixels[i] = pixel;
        }
    }

    T GetPixel(size_t x, size_t y) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            size_t i = x + y * width;
            return pixels[i];
        }
    }

    Texture() = default;
    Texture(int width, int height) {
        this->width = width;
        this->height = height;

        pixels = std::vector<T>(width * height);
    };

    void Fill(T value) {
        for (size_t i = 0; i < (size_t)(width * height); i++) {
            pixels[i] = value;
        }
    }
};
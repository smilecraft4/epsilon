#pragma once
#include <cstdint>

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color(uint32_t color) {
        r = (std::uint8_t)(color >> 24);
        g = (std::uint8_t)(color >> 16);
        b = (std::uint8_t)(color >> 8);
        a = (std::uint8_t)(color);
    }
};
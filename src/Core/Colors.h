#pragma once

#include <cstdint>

#include "Maths.h"

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color(uint32_t color = 0x000000FF) {
        this->r = (std::uint8_t)(color >> 24);
        this->g = (std::uint8_t)(color >> 16);
        this->b = (std::uint8_t)(color >> 8);
        this->a = (std::uint8_t)(color);
    }

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    static Color Lerp(const Color &a, const Color &b, float t) {
        Color c{};
        c.r = LerpUint8_t(a.r, b.r, t);
        c.g = LerpUint8_t(a.g, b.g, t);
        c.b = LerpUint8_t(a.b, b.b, t);
        c.a = LerpUint8_t(a.a, b.a, t);
        return c;
    }
};
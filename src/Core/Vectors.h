#pragma once

#include <cstdint>

#include "Maths.h"

struct Vector2Int {
    int x;
    int y;
};

struct Vector3Int {
    int x;
    int y;
    int z;
};

struct Vector4Int {
    int x;
    int y;
    int z;
    int w;
};

struct Vector2 {
    float x;
    float y;
};

struct Vector3 {
    float x;
    float y;
    float z;

    static Vector3 Lerp(const Vector3 &a, const Vector3 &b, float t) {
        Vector3 c{};
        c.x = Lerpf(a.x, b.x, t);
        c.y = Lerpf(a.y, b.y, t);
        c.z = Lerpf(a.z, b.z, t);
        return c;
    }

    static float Distance(const Vector3 &a, const Vector3 &b) {
        float x = (b.x - a.x);
        float y = (b.y - a.y);
        float z = (b.z - a.z);
        float d = sqrt(x * x + y * y + z * z);
        return d;
    }
};

struct Vector4 {
    float x;
    float y;
    float z;
    float w;
};
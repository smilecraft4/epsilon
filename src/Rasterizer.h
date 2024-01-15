#pragma once

#include <memory>

#include "Core/Vectors.h"
#include "Core/Colors.h"
#include "Core/Maths.h"
#include "Screen.h"

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
    Color color;
};

struct Viewport {
    float far_clip;
    float near_clip;
};

class Rasterizer {
  public:
    Rasterizer(std::shared_ptr<Screen> screen);
    ~Rasterizer() = default;

    void DrawLine(Vertex a, Vertex b);
    void DrawTriangle(/**/);
    void DrawMesh(/**/);

  private:
    float far_clip;
    float near_clip;
};
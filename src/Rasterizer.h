#pragma once

#include <memory>

#include "Core/Colors.h"
#include "Core/Maths.h"
#include "Core/Vectors.h"
#include "Texture.h"

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
    Color color;

    // TODO: Consider using DOD for this so a container of positins and another of normals etc...
    static Vertex Lerp(const Vertex &a, const Vertex &b, float t) {
        Vertex c;
        c.position = Vector3::Lerp(a.position, b.position, t);
        c.normal = Vector3::Lerp(a.normal, b.normal, t);
        c.uv = Vector2::Lerp(a.uv, b.uv, t);
        c.color = Color::Lerp(a.color, b.color, t);
        return c;
    }
};

struct Viewport {
    float far_clip;
    float near_clip;
};

class Rasterizer {
  public:
    Rasterizer();
    ~Rasterizer() = default;

    void DrawLine(Vertex a, Vertex b);
    void DrawTriangle(Vertex a, Vertex b, Vertex c);
    void DrawMesh(/**/);

  private:
    std::unique_ptr<Texture<uint32_t>> color_buffer_;
    std::unique_ptr<Texture<uint16_t>> depth_buffer_;

    float far_clip;
    float near_clip;
};
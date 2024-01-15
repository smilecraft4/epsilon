#include "Rasterizer.h"

const Texture<Color> *Rasterizer::ColorBuffer() { return color_buffer_.get(); }

Vector3 Rasterizer::ViewportToScreenspace(Vector3 viewport) {
    // viewport size
    /*

    +----------------------------------------+                     +-----------------------------------------+
    |(-1.0, 1.0)                  ( 1.0, 1.0)|                     |(0, height-1)         (width-1, height-1)|
    |                                        |                     |                                         |
    |                                        |                     |                                         |
    |                                        |                     |                                         |
    |                                        |                     |                                         |
    |               (0.0, 0.0)               |   --------->        |          (width-1/2, height-1/2)        |
    |                                        |                     |                                         |
    |                                        |                     |                                         |
    |                                        |                     |                                         |
    |                                        |                     |                                         |
    |(-1.0,-1.0)                  ( 1.0,-1.0)|                     |(0,0)                          width-1,0)|
    +----------------------------------------+                     +-----------------------------------------+

    */

    // -1.0     x                               1.0
    // 0.0      x+1.0                           2.0
    // 0.0      (x+1.0)/2.0                     1.0
    // 0.0      ((x+1.0)/2.0)*(width-1)         width-1

    viewport.x = ((viewport.x + 1.0) / 2.0) * (color_buffer_->width - 1);
    viewport.y = ((viewport.y + 1.0) / 2.0) * (color_buffer_->height - 1);

    return viewport;
}

Rasterizer::Rasterizer(int width, int height) : width_(width), height_(height) {
    color_buffer_ = std::make_unique<Texture<Color>>(width_, height_);
    color_buffer_->Fill(Color(0, 0, 0));
}

void Rasterizer::Clear() { color_buffer_->Fill({0, 0, 0}); }

void Rasterizer::DrawVertex(Vertex a) {
    a.position = ViewportToScreenspace(a.position);
    color_buffer_->SetPixel(a.position.x, a.position.y, a.color);
}

void Rasterizer::DrawLine(Vertex a, Vertex b) {
    // get the vertices as his and clip them to avoid unacessery computation
    // project the vertex from viewport to screen space

    color_buffer_->pixels[4] = Color(0, 0, 0);
}

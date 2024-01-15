#include "Rasterizer.h"

Vector3 ViewportToScreenspace(Vector3 viewport) {
    Vector3 screen;

    // viewport range

    return screen;
}

void Rasterizer::DrawLine(Vertex a, Vertex b) {
    // get the vertices as his and clip them to avoid unacessery computation
    // project the vertex from viewport to screen space


    color_buffer_->pixels[4] = 0xFFFF00FF;
    depth_buffer_->pixels[4] = 0xFFFF;
}

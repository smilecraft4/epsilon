#include "Rasterizer.h"

const Texture<Color> *Rasterizer::ColorBuffer() { return color_buffer_.get(); }

Vector3 Rasterizer::ViewportToScreenspace(Vector3 viewport, bool clamp) {
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

    (near_clip)                     (far_clip)                      (0.0)                                (1.0)
    +----------------------------------------+                      +----------------------------------------+

    */

    // -1.0     x                               1.0
    // 0.0      x+1.0                           2.0
    // 0.0      (x+1.0)/2.0                     1.0
    // 0.0      ((x+1.0)/2.0)*(width-1)         width-1

    // near         z                           far
    // 0.0          z-near                      far - near
    // 0.0          (z-near)(far-near)          1.0

    viewport.x = floor(((viewport.x + 1.0) / 2.0) * (color_buffer_->width - 1));
    viewport.y = floor(((viewport.y + 1.0) / 2.0) * (color_buffer_->height - 1));
    viewport.z = (viewport.z - near_clip) / (far_clip - near_clip);

    return viewport;
}

Rasterizer::Rasterizer(int width, int height) : width_(width), height_(height) {
    color_buffer_ = std::make_unique<Texture<Color>>(width_, height_);
    color_buffer_->Fill(Color(0, 0, 0));

    depth_buffer_ = std::make_unique<Texture<uint16_t>>(width_, height_);
    depth_buffer_->Fill(0xFFFFu);

    far_clip = 5.0f;
    near_clip = -5.0f;
}

static float sign(float val) { return (0.0f < val) - (val < 0.0f); }

void Rasterizer::Clear() {
    color_buffer_->Fill({0, 0, 0});
    depth_buffer_->Fill(0xFFFFu);
}

void Rasterizer::DrawVertex(Vertex a) {
    a.position = ViewportToScreenspace(a.position);
    color_buffer_->SetPixel(a.position.x, a.position.y, a.color);
}

void Rasterizer::DrawLine(Vertex a, Vertex b) {
    // get the vertices as his and clip them to avoid unacessery computation
    // project the vertex from viewport to screen space
    const Vector3 a_s = ViewportToScreenspace(a.position);
    const Vector3 b_s = ViewportToScreenspace(b.position);

    const float delta_x = b_s.x - a_s.x;
    const float delta_y = b_s.y - a_s.y;

    const float x_direction = sign(delta_x);
    const float y_direction = sign(delta_y);

    a.color = a.color * (1.0 - a_s.z);
    b.color = b.color * (1.0 - b_s.z);

    if (delta_x == 0) {
        if (delta_y == 0) {
            if (depth_buffer_->GetPixel(a_s.y, a_s.y) <= (a_s.z * (UINT16_MAX))) {
                return;
            }
            depth_buffer_->SetPixel(a_s.x, a_s.y, a_s.z);
            color_buffer_->SetPixel(a_s.x, a_s.y, a.color);
        } else {
            for (int i = 0; i < delta_y * y_direction; i++) {
                const float y = i * y_direction;

                uint16_t depth = std::lerp(a_s.z, b_s.z, y / delta_y) * (UINT16_MAX);
                if (depth_buffer_->GetPixel(a_s.y, a_s.y + y) <= depth) {
                    return;
                }
                depth_buffer_->SetPixel(a_s.x, a_s.y + y, depth);

                color_buffer_->SetPixel(a_s.x, a_s.y + y, Color::Lerp(a.color, b.color, y / delta_y));
            }
        }
    }
    if (delta_y == 0) {
        for (int i = 0; i < delta_x * x_direction; i++) {
            const float x = i * x_direction;

            uint16_t depth = std::lerp(a_s.z, b_s.z, x / delta_x) * (UINT16_MAX);
            if (depth_buffer_->GetPixel(a_s.x + x, a_s.y) <= depth) {
                return;
            }
            depth_buffer_->SetPixel(a_s.x + x, a_s.y, depth);

            color_buffer_->SetPixel(a_s.x + x, a_s.y, Color::Lerp(a.color, b.color, x / delta_x));
            // color_buffer_->SetPixel(a_s.x + x, a_s.y, col);
        }
    }

    if (abs(delta_x) > abs(delta_y)) {
        // biggest increment is delta_x so going for y = mx+a
        const float slope = delta_y / delta_x;
        for (int i = 0; i < delta_x * x_direction; i++) {
            float x = i * x_direction;

            uint16_t depth = std::lerp(a_s.z, b_s.z, x / delta_x) * (UINT16_MAX);
            if (depth_buffer_->GetPixel(a_s.x + x, a_s.y + slope * x) <= depth) {
                return;
            }
            depth_buffer_->SetPixel(a_s.x + x, a_s.y + slope * x, depth);

            color_buffer_->SetPixel(a_s.x + x, a_s.y + slope * x, Color::Lerp(a.color, b.color, x / delta_x));
            // color_buffer_->SetPixel(a_s.x + x, a_s.y + slope * x, col);
        }
    } else {
        // biggest increment is delta_y so going for x = my+a
        const float slope = delta_x / delta_y;
        for (int i = 0; i < delta_y * y_direction; i++) {
            float y = i * y_direction;

            uint16_t depth = std::lerp(a_s.z, b_s.z, y / delta_y) * (UINT16_MAX);
            if (depth_buffer_->GetPixel(a_s.x + slope * y, a_s.y + y) <= depth) {
                return;
            }
            depth_buffer_->SetPixel(a_s.x + slope * y, a_s.y + y, depth);

            color_buffer_->SetPixel(a_s.x + slope * y, a_s.y + y, Color::Lerp(a.color, b.color, y / delta_y));
            // color_buffer_->SetPixel(a_s.x + slope * y, a_s.y + y, col);
        }
    }
}

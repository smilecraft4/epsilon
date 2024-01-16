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

    */

    // -1.0     x                               1.0
    // 0.0      x+1.0                           2.0
    // 0.0      (x+1.0)/2.0                     1.0
    // 0.0      ((x+1.0)/2.0)*(width-1)         width-1

    viewport.x = floor(((viewport.x + 1.0) / 2.0) * (color_buffer_->width - 1));
    viewport.y = floor(((viewport.y + 1.0) / 2.0) * (color_buffer_->height - 1));

    return viewport;
}

Rasterizer::Rasterizer(int width, int height) : width_(width), height_(height) {
    color_buffer_ = std::make_unique<Texture<Color>>(width_, height_);
    color_buffer_->Fill(Color(0, 0, 0));
}

static float sign(float val) { return (0.0f < val) - (val < 0.0f); }

void Rasterizer::Clear() { color_buffer_->Fill({0, 0, 0}); }

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

    const Color col(255, 255, 0);

    if (delta_x == 0) {
        if (delta_y == 0) {
            color_buffer_->SetPixel(a_s.x, a_s.y, a.color);
        } else {
            for (int i = 0; i < delta_y * y_direction; i++) {
                const float y = i * y_direction;
                //color_buffer_->SetPixel(a_s.x, a_s.y + y, Color::Lerp(a.color, b.color, y / delta_y));
                color_buffer_->SetPixel(a_s.x, a_s.y + y, col);
            }
        }
    }
    if (delta_y == 0) {
        for (int i = 0; i < delta_x * x_direction; i++) {
            const float x = i * x_direction;
            //color_buffer_->SetPixel(a_s.x + x, a_s.y, Color::Lerp(a.color, b.color, x / delta_x));
            color_buffer_->SetPixel(a_s.x + x, a_s.y, col);
        }
    }

    if (abs(delta_x) > abs(delta_y)) {
        // biggest increment is delta_x so going for y = mx+a
        const float slope = delta_y / delta_x;
        for (int i = 0; i < delta_x * x_direction; i++) {
            float x = i * x_direction;

            //color_buffer_->SetPixel(a_s.x + x, a_s.y + slope * x, Color::Lerp(a.color, b.color, x / delta_x));
            color_buffer_->SetPixel(a_s.x + x, a_s.y + slope * x, col);
        }
    } else {
        // biggest increment is delta_y so going for x = my+a
        const float slope = delta_x / delta_y;
        for (int i = 0; i < delta_y * y_direction; i++) {
            float y = i * y_direction;

            //color_buffer_->SetPixel(a_s.x + slope * y, a_s.y + y, Color::Lerp(a.color, b.color, y / delta_y));
            color_buffer_->SetPixel(a_s.x + slope * y, a_s.y + y, col);
        }
    }

    // draw a straigth line
    // if (a_s.y == b_s.y) {
    //     // Same position
    //     if (a_s.x == b_s.x) {
    //         color_buffer_->SetPixel(a_s.x, a_s.y, a.color);
    //     }
    //     for (float x = 0; x < abs(b_s.x - a_s.x); x++) {
    //         float t = x / abs(b_s.x - a_s.x);
    //         if (a_s.x > b_s.x) {
    //             t = 1 - t;
    //         }
    //         Color col = Color::Lerp(a.color, b.color, t);
    //
    //         color_buffer_->SetPixel(fmin(b_s.x, a_s.x) + x, a_s.y, col);
    //     }
    // }
    // if (a_s.x == b_s.x) {
    //     for (float y = 0; y < abs(b_s.y - a_s.y); y++) {
    //         float t = y / abs(b_s.y - a_s.y);
    //         // TODO: Reorder vertex from lowest to biggest based on this
    //         if (a_s.y > b_s.y) {
    //             t = 1 - t;
    //         }
    //         Color col = Color::Lerp(a.color, b.color, t);
    //
    //         color_buffer_->SetPixel(a_s.x, fmin(b_s.y, a_s.y) + y, col);
    //     }
    // }

    // if (abs(b_s.x - a_s.x) > abs(b_s.y - a_s.y)) {
    //     // x > y slope
    //     // as y is smaller than x the value with the most "prescion" is x
    //     float slope = (b_s.y - a_s.y) / (b_s.x - a_s.x);
    //     for (float x = 0; x < abs(b_s.x - a_s.x); x++) {
    //         float t = x / abs(b_s.x - a_s.x);
    //         if (a_s.x > b_s.x) {
    //             t = 1 - t;
    //         }
    //         Color col = Color::Lerp(a.color, b.color, t);
    //
    //         color_buffer_->SetPixel(fmin(b_s.x, a_s.x) + x, fmin(b_s.y, a_s.y) + (x * slope), col);
    //     }
    //
    // } else {
    //     // y > x slope
    //     float slope = (b_s.x - a_s.x) / (b_s.y - a_s.y);
    //     for (float y = 0; y < abs(b_s.y - a_s.y); y++) {
    //         float t = y / abs(b_s.y - a_s.y);
    //         if (a_s.y > b_s.y) {
    //             t = 1 - t;
    //         }
    //         Color col = Color::Lerp(a.color, b.color, t);
    //
    //         color_buffer_->SetPixel(a_s.x + y * slope, fmin(a_s.y, a_s.y) + y, col);
    //     }
    // }

    const float slope = (b_s.x - a_s.x) / (b_s.y - a_s.y);

    color_buffer_->pixels[4] = Color(0, 0, 0);
}

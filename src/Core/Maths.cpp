#include "Maths.h"


// TODO: add simd instructions
// TODO: make sure nothing goes wrong when t > 1 or t < 0
float Lerpf(float a, float b, float t) {
    float c = a * (1.0f - t) + b * t;
    return c;
}

// TODO: does this work ?
uint8_t LerpUint8_t(uint8_t a, uint8_t b, float t) {
    uint8_t c = a * (1.0f - t) + b * t;
    return c;
}
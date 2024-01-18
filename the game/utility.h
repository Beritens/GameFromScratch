#pragma once
#include <cstdlib>

struct float2 {
    float x, y;

    float2() = default;
    float2(float x, float y) : x{ x }, y{ y } {}
    explicit float2(float s) : x{ s }, y{ s } {}
};

static float2 operator+(const float2& a, const float2& b) {
    return { a.x + b.x, a.y + b.y };
}
static float2 operator-(const float2& a, const float2& b) {
    return { a.x - b.x, a.y - b.y };
}

static float2 operator*(const float2& a, float s) {
    return { a.x * s, a.y * s };
}
static float2 operator/(const float2& a, float s) {
    return { a.x / s, a.y / s };
}

static float dot(const float2& a, const float2& b) {
    return a.x * b.x + a.y * b.y;
}
static float length(const float2& a) {
    return sqrt(a.x * a.x + a.y * a.y);
}
static float2 norm(float2 a) {
    return a / length(a);
}


static float randf() {
    return float(rand() / double(RAND_MAX));
}

static float randf(float a, float b) {
    return a + (b - a) * randf();
}

static float2 rand_dir(float scale = 1.f) {
    for (;;) {
        float2 v(randf(-1.f, 1.f), randf(-1.f, 1.f));
        if (dot(v, v) <= 1.f)
            return v * scale;
    }
}

#pragma once
#include "raylib.h"

struct BodySegment {
    Vector2 start, end;
    float length;
    float angle;           // in degrees
    float minAngle, maxAngle;
    BodySegment* parent;

    BodySegment(float len, float minA, float maxA, BodySegment* p = nullptr)
        : length(len), minAngle(minA), maxAngle(maxA), parent(p) {
        angle = 0;
        start = {0, 0};
        end   = {0, -len};
    }

    void update() {
        if (parent) start = parent->end;
        float rad = DEG2RAD * angle;
        end = {
            start.x + cosf(rad)*length,
            start.y + sinf(rad)*length
        };
    }

    void draw() const {
        DrawLineV(start, end, DARKBLUE);
    }
};

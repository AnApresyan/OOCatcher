#pragma once
#include "raylib.h"
#include <cmath>

class Target
{
public:
    Vector2 pos;
    float radius;
    Color color;

    Target(Vector2 pos, float radius, Color color)
        : pos(pos), radius(radius), color(color) {}

    void draw() const
    {
        int points = 5;
        Vector2 star[5];
        float angleOff = -PI / 2;
        for (int i = 0; i < points; ++i)
        {
            float ang = angleOff + i * 2 * PI / points;
            star[i] = {pos.x + radius * cosf(ang), pos.y + radius * sinf(ang)};
        }

        int order[6] = {0, 2, 4, 1, 3, 0};

        for (int i = 0; i < 5; ++i)
            DrawLineV(star[order[i]], star[order[i + 1]], color);

        for (int i = 0; i < 5; ++i)
            DrawCircleV(star[i], 3, color);
    }
    Vector2 getPos() const { return pos; }
};

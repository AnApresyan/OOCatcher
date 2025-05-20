#pragma once

#include "raylib.h"

class Target
{
public:
    Target(Vector2 pos, float radius, Color color);

    void draw() const;
    Vector2 getPos() const;

    // raw data kept public to preserve existing usage
    Vector2 pos;
    float radius;
    Color color;
};

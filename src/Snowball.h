#pragma once
#include "raylib.h"
#include "raymath.h"    // for Vector2Normalize()
#include "Shapes.h"     // your Circle definition
#include "OOCatcher.h"

class Snowball : public OOCatcher {
    Circle target;
    bool thrown = false;
    Vector2 velocity;
public:
    Snowball(const Circle& t): target(t) {}
    void init() override;
    void step() override;
    void draw() override;
};

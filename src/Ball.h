#pragma once

#include "raylib.h"

enum BallState
{
    BALL_ON_GROUND,
    BALL_HELD,
    BALL_THROWN,
    BALL_FALLING,
    BALL_AT_REST
};

class Ball
{
public:
    Ball(Vector2 center, float radius, Color color);

    void hold(const Vector2 &handPos);
    void throwTo(const Vector2 &start, const Vector2 &target, float gravity, float timeToTarget);
    void update(float gravity, float dt, float groundY);
    void draw() const;

    Vector2 getPos() const;
    float getRadius() const;
    void setState(BallState s);

    // raw data kept public to preserve existing behavior
    Vector2 pos;
    Vector2 vel;
    float radius;
    Color color;
    BallState state;
};

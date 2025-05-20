#pragma once
#include "raylib.h"

enum BallState { BALL_ON_GROUND, BALL_HELD, BALL_THROWN, BALL_FALLING, BALL_AT_REST };

class Ball {
public:
    Vector2 pos, vel;
    float radius;
    Color color;
    BallState state;

    Ball(Vector2 center, float radius, Color color)
        : pos(center), vel({0,0}), radius(radius), color(color), state(BALL_ON_GROUND) {}

    void hold(const Vector2& handPos) {
        pos = handPos;
        vel = {0,0};
        state = BALL_HELD;
    }

    void throwTo(const Vector2& start, const Vector2& target, float gravity, float timeToTarget) {
        pos = start;
        vel = { (target.x - start.x) / timeToTarget,
                (target.y - start.y - 0.5f*gravity*timeToTarget*timeToTarget) / timeToTarget };
        state = BALL_THROWN;
    }

    void update(float gravity, float dt, float groundY) {
        if (state == BALL_THROWN || state == BALL_FALLING) {
            vel.y += gravity * dt;
            pos.x += vel.x * dt;
            pos.y += vel.y * dt;

            if (state == BALL_THROWN && vel.y > 0 && pos.y > groundY - radius) {
                // Hit ground after flying
                pos.y = groundY - radius;
                vel = {0,0};
                state = BALL_AT_REST;
            } else if (state == BALL_FALLING && pos.y > groundY - radius) {
                // After missed or after reaching the target and falling
                pos.y = groundY - radius;
                vel = {0,0};
                state = BALL_AT_REST;
            }
        }
    }

    void draw() const {
        DrawCircleV(pos, radius, Fade(color, 0.25f));
        DrawCircleLines((int)pos.x, (int)pos.y, (int)radius, color);
    }
};

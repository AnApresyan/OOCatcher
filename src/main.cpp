#include "raylib.h"
#include "Walker.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include "Target.h"

int main() {
    const int screenW = 900, screenH = 600;
    InitWindow(screenW, screenH, "OOCatcher");
    SetTargetFPS(60);

    float groundY = 500.0f;
    std::srand(std::time(nullptr));

    std::vector<Color> possibleColors = { RED, ORANGE, VIOLET, GREEN, BLUE, GOLD, DARKPURPLE, PINK, BROWN };

    struct Ball {
        Vector2 center;
        float radius;
        Color color;
    };

    auto randomBall = [&]() {
        float x = 400 + rand() % (screenW - 500);
        float groundYBall = groundY - (rand() % 4);
        float lowY = groundY - 50.0f - (rand() % 25);
        float highY = groundY - 140.0f - (rand() % 25);
        int mode = rand() % 3;
        float y = (mode == 0) ? groundYBall : (mode == 1) ? lowY : highY;
        float radius = 16.0f + rand() % 18;
        Color color = possibleColors[rand() % possibleColors.size()];
        return Ball{ {x, y}, radius, color };
    };

    auto randomTarget = [&]() {
        float x = 100 + rand() % (screenW - 200);
        float y = 100 + rand() % (int)(groundY - 200);
        float radius = 22.0f + rand() % 15;
        Color color = possibleColors[rand() % possibleColors.size()];
        return Target({x, y}, radius, color);
    };

    Target target = randomTarget();
    bool targetActive = false;

    Ball ball = randomBall();
    Walker walker(ball.center, ball.radius);
    walker.init();
    Color currentBallColor = ball.color;

    bool stickmanShouldStand = false; 
    bool ballTouched = false;         

    // Throwing variables
    bool ballFlying = false;
    Vector2 ballVelocity = {0, 0};
    float gravity = 750.0f;
    float timeSinceThrow = 0.0f;
    float flightTime = 0.9f;

    bool throwAnimating = false;
    float throwAnimTime = 0.0f;
    float throwAnimDuration = 0.40f;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            ball = randomBall();
            walker = Walker(ball.center, ball.radius);
            walker.init();
            currentBallColor = ball.color;
            stickmanShouldStand = false;
            ballTouched = false;
            targetActive = false;
            ballFlying = false;
            timeSinceThrow = 0.0f;
            ballVelocity = {0, 0};
        }

        walker.step();

        if (!ballTouched && walker.fingersTouchingBall()) {
            ballTouched = true;
        }

        if (ballTouched && !stickmanShouldStand && IsKeyPressed(KEY_SPACE)) {
            stickmanShouldStand = true;
            target = randomTarget();
            targetActive = true;
        }

        walker.setStandUp(stickmanShouldStand);

        if (stickmanShouldStand && !ballFlying) {
            ball.center = walker.getHandPos();
        }

        if (targetActive && stickmanShouldStand && !ballFlying && IsKeyPressed(KEY_T)) {
            throwAnimating = true;
            throwAnimTime = 0.0f;
            
            // Vector2 start = walker.getHandPos();
            // Vector2 end = target.pos;
            // ballVelocity.x = (end.x - start.x) / flightTime;
            // ballVelocity.y = (end.y - start.y - 0.5f * gravity * flightTime * flightTime) / flightTime;
            // ballFlying = true;
            // timeSinceThrow = 0.0f;
        }
        if (throwAnimating) {
            throwAnimTime += GetFrameTime();
            float phase = fminf(throwAnimTime / throwAnimDuration, 1.0f);
            walker.setThrowAnim(1.0f, phase);
            ball.center = walker.getHandPos();
        
            if (phase >= 1.0f) {
                Vector2 start = walker.getHandPos();
                Vector2 end = target.pos;
                ballVelocity.x = (end.x - start.x) / flightTime;
                ballVelocity.y = (end.y - start.y - 0.5f * gravity * flightTime * flightTime) / flightTime;
                ballFlying = true;
                throwAnimating = false;
                timeSinceThrow = 0.0f;
                walker.setThrowAnim(0.0f, 0.0f);
            }
        } else if (stickmanShouldStand && !ballFlying) {
            walker.setThrowAnim(0.0f, 0.0f);
            ball.center = walker.getHandPos();
        }

        if (ballFlying) {
            float dt = GetFrameTime();
            timeSinceThrow += dt;
            ball.center.x += ballVelocity.x * dt;
            ball.center.y += ballVelocity.y * dt;
            ballVelocity.y += gravity * dt;

            // Land on ground and stop
            if (ball.center.y >= groundY - ball.radius) {
                ball.center.y = groundY - ball.radius;
                ballFlying = false;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawLine(0, (int)groundY, screenW, (int)groundY, BLACK);

        walker.draw();

        DrawCircleV(ball.center, ball.radius, Fade(currentBallColor, 0.2f));
        DrawCircleLines((int)ball.center.x, (int)ball.center.y, (int)ball.radius, currentBallColor);

        if (targetActive)
            target.draw();

        DrawText("Walker Mode", 10, 10, 20, GRAY);
        DrawText("Press R to Restart", 10, 35, 18, DARKGRAY);

        if (ballTouched && !stickmanShouldStand)
            DrawText("Press SPACE to Activate Throwing Mode!", 10, 60, 22, BLUE);
        if (targetActive && stickmanShouldStand && !ballFlying)
            DrawText("Press T to Throw Ball at Star!", 10, 90, 22, RED);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

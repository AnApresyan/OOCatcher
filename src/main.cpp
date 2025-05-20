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

    bool stickmanShouldStand = false; // Only stands after pressing SPACE after touching
    bool ballTouched = false;         // Track if ball is already touched

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            ball = randomBall();
            walker = Walker(ball.center, ball.radius);
            walker.init();
            currentBallColor = ball.color;
            stickmanShouldStand = false;
            ballTouched = false;
            targetActive = false;
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
        if (targetActive)
            target.draw();

        walker.setStandUp(stickmanShouldStand);

        if (stickmanShouldStand) {
            ball.center = walker.getHandPos(); 
        }


        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawLine(0, (int)groundY, screenW, (int)groundY, BLACK);

        walker.draw();

        DrawCircleV(ball.center, ball.radius, Fade(currentBallColor, 0.2f));

        DrawText("Walker Mode", 10, 10, 20, GRAY);
        DrawText("Press R to Restart", 10, 35, 18, DARKGRAY);

        if (ballTouched && !stickmanShouldStand)
            DrawText("Press SPACE to Activate Throwing Mode!", 10, 60, 22, BLUE);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

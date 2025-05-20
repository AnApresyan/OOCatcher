#include "raylib.h"
#include "Walker.h"
#include <cstdlib>
#include <ctime>
#include <vector>

int main() {
    const int screenW = 900, screenH = 600;
    InitWindow(screenW, screenH, "OOCatcher");
    SetTargetFPS(60);

    float groundY = 500.0f;
    std::srand(std::time(nullptr));

    // Ball randomizer: sometimes high, sometimes low, size and color random
    std::vector<Color> possibleColors = { RED, ORANGE, VIOLET, GREEN, BLUE, GOLD, DARKPURPLE, PINK, BROWN };

    struct Ball {
        Vector2 center;
        float radius;
        Color color;
    };

    auto randomBall = [&]() {
        float x = 400 + rand() % (screenW - 500);
    
        // Three possible heights: high, low, and ON the ground
        float groundYBall = groundY - (rand() % 4); // 0–3 pixels above ground
        float lowY = groundY - 50.0f - (rand() % 25);
        float highY = groundY - 140.0f - (rand() % 25);
    
        int mode = rand() % 3;
        float y = (mode == 0) ? groundYBall : (mode == 1) ? lowY : highY;
    
        float radius = 16.0f + rand() % 18;
        Color color = possibleColors[rand() % possibleColors.size()];
        return Ball{ {x, y}, radius, color };
    };
    

    Ball ball = randomBall();

    Walker walker(ball.center, ball.radius);
    walker.init();

    // Walker will need to know the current color for drawing the ball
    Color currentBallColor = ball.color;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            ball = randomBall();
            walker = Walker(ball.center, ball.radius);
            walker.init();
            currentBallColor = ball.color;
        }

        walker.step();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawLine(0, (int)groundY, screenW, (int)groundY, BLACK);

        walker.draw();

        // Draw the ball in its (possibly new) color and size
        DrawCircleV(ball.center, ball.radius, Fade(currentBallColor, 0.2f));
        DrawCircleLines((int)ball.center.x, (int)ball.center.y, (int)ball.radius, currentBallColor);

        DrawText("Walker Mode", 10, 10, 20, GRAY);
        DrawText("Press R to Restart", 10, 35, 18, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

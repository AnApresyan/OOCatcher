#include "raylib.h"
#include "Walker.h"
#include <cstdlib>
#include <ctime>

int main() {
    const int screenW = 900, screenH = 600;
    InitWindow(screenW, screenH, "OOCatcher");
    SetTargetFPS(60);

    float groundY = 500.0f;
    float ballRadius = 32.0f;
    std::srand(std::time(nullptr)); // initialize random seed

    // Ball randomizer: sometimes high, sometimes low
    auto randomBallCenter = [&]() {
        float x = 400 + rand() % (screenW - 500); // [400, screenW-100]
        float lowY = groundY - 50.0f - (rand() % 25);      // low (waist/chest)
        float highY = groundY - 140.0f - (rand() % 25);    // high (overhead)
        float y = (rand() % 2 == 0) ? lowY : highY;        // 50% high/low
        return Vector2{ x, y };
    };

    Vector2 ballCenter = randomBallCenter();

    Walker walker(ballCenter, ballRadius);
    walker.init();

    while (!WindowShouldClose()) {
        // Restart with a new random ball if R is pressed
        if (IsKeyPressed(KEY_R)) {
            ballCenter = randomBallCenter();
            walker = Walker(ballCenter, ballRadius);
            walker.init();
        }

        walker.step();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw ground
        DrawLine(0, (int)groundY, screenW, (int)groundY, BLACK);
        walker.draw();

        DrawText("Walker Mode", 10, 10, 20, GRAY);
        DrawText("Press R to Restart", 10, 35, 18, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

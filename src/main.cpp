#include "raylib.h"
#include "Walker.h"
#include <memory>

int main() {
    const int screenW = 900, screenH = 600;
    InitWindow(screenW, screenH, "OOCatcher");
    SetTargetFPS(60);

    float groundY = 500.0f;
    float ballRadius = 32.0f;
    std::srand(std::time(nullptr)); // initialize random seed

    // Function to get a random ball position (not too close to the left side)
    auto randomBallCenter = [&]() {
        float x = 400 + rand() % (screenW - 500); // [400, screenW-100]
        float y = groundY - 50.0f - (rand() % 60); // somewhere above ground
        return Vector2{ x, y };
    };

    Vector2 ballCenter = randomBallCenter();

    Walker walker(ballCenter, ballRadius);
    walker.init();

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            ballCenter = randomBallCenter();
            walker = Walker(ballCenter, ballRadius); // Re-create with new ball
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
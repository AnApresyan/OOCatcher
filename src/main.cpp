#include "raylib.h"
#include "Walker.h"
#include <memory>

int main() {
    const int screenW = 900, screenH = 600;
    InitWindow(screenW, screenH, "OOCatcher");
    SetTargetFPS(60);

    float groundY = 500.0f;
    Vector2 ballCenter = { 650.0f, groundY - 50.0f };
    float ballRadius = 32.0f;

    Walker walker(ballCenter, ballRadius);
    walker.init();

    while (!WindowShouldClose()) {
        // Restart if R is pressed
        if (IsKeyPressed(KEY_R)) {
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

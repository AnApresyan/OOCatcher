#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

Game::Game(int w, int h)
    : screenW(w)
    , screenH(h)
    , groundY(500.0f)
    , possibleColors{ RED, ORANGE, VIOLET, GREEN, BLUE, GOLD, DARKPURPLE, PINK, BROWN }
    , ball(randomBall())
    , walker(ball.getPos(), ball.getRadius())
    , target({0,0}, 0, WHITE)

    , flightTime(0.9f)
    , gravity(750.0f)
    , throwAnimDuration(0.4f)
    , throwAnimTime(0.0f)
{
    initWindow();
    initGame();
}

Game::~Game() {
    CloseWindow();
}

void Game::initWindow() {
    InitWindow(screenW, screenH, "OOCatcher");
    SetTargetFPS(60);
}

void Game::initGame() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    walker.init();
}

void Game::run() {
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        processInput();
        update(dt);
        draw();
    }
}

void Game::processInput() {
    if (IsKeyPressed(KEY_R)) {
        reset();
        return;
    }

    if (!ballTouched && walker.fingersTouchingBall()) {
        ballTouched = true;
    }

    if (ballTouched && !stickmanStand && IsKeyPressed(KEY_SPACE)) {
        stickmanStand = true;
        target        = randomTarget();
        targetActive  = true;
    }

    walker.setStandUp(stickmanStand);

    if (targetActive && stickmanStand && !ballFlying && IsKeyPressed(KEY_T)) {
        throwAnimating  = true;
        throwAnimTime   = 0.0f;
    }
}

void Game::update(float dt) {
    walker.step();

    if (stickmanStand && !ballFlying && !throwAnimating) {
        ball.hold(walker.getHandPos());
    }

    if (throwAnimating) {
        throwAnimTime += dt;
        float phase = std::min(throwAnimTime / throwAnimDuration, 1.0f);
        walker.setThrowAnim(1.0f, phase);
        ball.hold(walker.getHandPos());

        if (phase >= 1.0f) {
            Vector2 start = walker.getHandPos();
            ball.throwTo(start, target.getPos(), gravity, flightTime);
            ballFlying     = true;
            throwAnimating = false;
            walker.setThrowAnim(0.0f, 0.0f);
        }
    }

    if (ballFlying) {
        ball.update(gravity, dt, groundY);
        if (ball.getPos().y >= groundY - ball.getRadius()) {
            ballFlying = false;
            ball.setState(BALL_AT_REST);
        }
    }
}

void Game::draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // ground line
    DrawLine(0, (int)groundY, screenW, (int)groundY, BLACK);

    // scene
    walker.draw();
    ball.draw();
    if (targetActive) target.draw();

    // HUD
    DrawText("Press R to reset", 10, 10, 18, DARKGRAY);
    if (!ballTouched)
        DrawText("Move into the ball to grab it", 10, 35, 18, BLUE);
    else if (!stickmanStand)
        DrawText("Press SPACE to stand and aim", 10, 60, 18, BLUE);
    else if (!ballFlying)
        DrawText("Press T to throw", 10, 85, 18, RED);

    EndDrawing();
}

Ball Game::randomBall() {
    float r = 16.0f + std::rand() % 18;            
    float x = 400 + std::rand() % (screenW - 500); 

    const int maxVerticalOffset = 200; 

    int minY = static_cast<int>(groundY - r - maxVerticalOffset);
    int maxY = static_cast<int>(groundY - r);
    float y = static_cast<float>(minY + (std::rand() % (maxY - minY + 1)));

    Color c = possibleColors[ std::rand() % possibleColors.size() ];
    return Ball({ x, y }, r, c);
}


Target Game::randomTarget() {
    float x = 100 + std::rand() % (screenW - 200);
    float y = 100 + std::rand() % static_cast<int>(groundY - 200);
    float r = 22.0f + std::rand() % 15;
    Color c = possibleColors[std::rand() % possibleColors.size()];
    return Target({ x, y }, r, c);
}

void Game::reset() {
    ball          = randomBall();
    walker        = Walker(ball.getPos(), ball.getRadius());
    walker.init();
    targetActive  = false;
    stickmanStand = false;
    ballTouched   = false;
    ballFlying    = false;
}

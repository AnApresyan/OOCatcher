#pragma once
#include "raylib.h"
#include "Ball.h"
#include "Walker.h"
#include "Target.h"
#include <vector>

class Game
{
public:
	Game(int screenW, int screenH);
	~Game();

	void run();

private:
	// --- setup helpers ---
	void initWindow();
	void initGame();

	// --- per-frame ---
	void processInput();
	void update(float dt);
	void draw();

	// --- factory / reset ---
	Ball randomBall();
	Target randomTarget();
	void reset();

	// --- window & ground ---
	int screenW;
	int screenH;
	float groundY;

	// **declare colors first!**
	std::vector<Color> possibleColors;

	// now these come *after* possibleColors
	Ball ball;
	Walker walker;
	Target target;

	// game state
	bool targetActive = false;
	bool stickmanStand = false;
	bool ballTouched = false;
	bool ballFlying = false;
	bool throwAnimating = false;

	// physics / timing
	float flightTime;
	float gravity;
	float throwAnimDuration;
	float throwAnimTime;
};

#include "Ball.h"

Ball::Ball(Vector2 center, float radius, Color color)
	: pos(center), vel({0, 0}), radius(radius), color(color), state(BALL_ON_GROUND)
{
}

void Ball::hold(const Vector2 &handPos)
{
	pos = handPos;
	vel = {0, 0};
	state = BALL_HELD;
}

void Ball::throwTo(const Vector2 &start,
				   const Vector2 &target,
				   float gravity,
				   float timeToTarget)
{
	pos = start;
	vel.x = (target.x - start.x) / timeToTarget;
	vel.y = (target.y - start.y - 0.5f * gravity * timeToTarget * timeToTarget) / timeToTarget;
	state = BALL_THROWN;
}

void Ball::update(float gravity, float dt, float groundY)
{
	if (state == BALL_THROWN || state == BALL_FALLING)
	{
		vel.y += gravity * dt;
		pos.x += vel.x * dt;
		pos.y += vel.y * dt;

		if (state == BALL_THROWN && vel.y > 0 && pos.y > groundY - radius)
		{
			pos.y = groundY - radius;
			vel = {0, 0};
			state = BALL_AT_REST;
		}
		else if (state == BALL_FALLING && pos.y > groundY - radius)
		{
			pos.y = groundY - radius;
			vel = {0, 0};
			state = BALL_AT_REST;
		}
	}
}

void Ball::draw() const
{
	DrawCircleV(pos, radius, Fade(color, 0.25f));
}

Vector2 Ball::getPos() const
{
	return pos;
}

float Ball::getRadius() const
{
	return radius;
}

void Ball::setState(BallState s)
{
	state = s;
}

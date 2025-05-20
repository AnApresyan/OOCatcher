#include "Target.h"
#include <cmath>

Target::Target(Vector2 pos, float radius, Color color)
	: pos(pos), radius(radius), color(color)
{
}

void Target::draw() const
{
	constexpr int points = 5;
	Vector2 star[points];
	float angleOff = -PI / 2.0f;

	for (int i = 0; i < points; ++i)
	{
		float ang = angleOff + i * 2 * PI / points;
		star[i] = {
			pos.x + radius * cosf(ang),
			pos.y + radius * sinf(ang)};
	}

	const int order[points + 1] = {0, 2, 4, 1, 3, 0};
	for (int i = 0; i < points; ++i)
		DrawLineV(star[order[i]], star[order[i + 1]], color);

	for (int i = 0; i < points; ++i)
		DrawCircleV(star[i], 3, color);
}

Vector2 Target::getPos() const
{
	return pos;
}

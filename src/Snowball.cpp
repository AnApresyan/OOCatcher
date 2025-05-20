#include "Snowball.h"
#include <cmath>

void Snowball::init() {
    BodySegment* prev = nullptr;
    float lens[] = {50, 40, 30, 20, 10};
    float mins[] = {-90,-90,-90,-90,-90}, maxs[] = {90,90,90,90,90};
    for(int i = 0; i < 5; i++) {
        auto seg = new BodySegment(lens[i], mins[i], maxs[i], prev);
        segments.push_back(seg);
        prev = seg;
    }
    segments[0]->start = {400, 450};
    for(auto s : segments) s->update();
}

void Snowball::step() {
    if (!thrown) {
        // aim by rotating last segment
        BodySegment* end = segments.back();
        Vector2 dir = { target.x - end->start.x, target.y - end->start.y };
        float desired = RAD2DEG * atan2f(dir.y, dir.x);
        end->angle = fmaxf(end->minAngle, fminf(desired, end->maxAngle));
        for(auto s : segments) s->update();

        // check ready: all segments touching or below ground
        int grounded = 0;
        for(auto s : segments) {
            if (s->start.y >= 450 || s->end.y >= 450) grounded++;
        }
        if (grounded >= 5) {
            thrown = true;
            float speed = 300.0f;
            // build and normalize aim direction
            Vector2 aimDir = {
                cosf(DEG2RAD * end->angle),
                sinf(DEG2RAD * end->angle)
            };
            Vector2 dirNorm = Vector2Normalize(aimDir);
            velocity = { dirNorm.x * speed, dirNorm.y * speed };
            TraceLog(LOG_INFO, "Snowball thrown at (%f,%f) vel=(%f,%f)",
                     end->end.x, end->end.y, velocity.x, velocity.y);
        }
    } else {
        // simulate snowball arc
        float dt = GetFrameTime();
        segments.back()->end.x += velocity.x * dt;
        segments.back()->end.y += velocity.y * dt + 100 * dt * dt;
        velocity.y += 100 * dt;
    }
}

void Snowball::draw() {
    for(auto s : segments) s->draw();
    DrawCircleLines((int)target.x, (int)target.y, (int)target.radius, BLUE);
}

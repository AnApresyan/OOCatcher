#include "Walker.h"
#include <cmath>

Walker::Walker(Vector2 ballC, float ballR)
    : ballCenter(ballC), ballRadius(ballR) {}

void Walker::init() {
    time = 0.0f;
    walk_phase = 0.0f;
    left_stance = true;  // start with left leg planted
    walking = true;
    reached = false;

    position = { 100, 500 };
    baseY = 500.0f;
    speed = 2.0f;
    torsoLen = 80.0f;
    headRadius = 18.0f;
    footLift = 18.0f;

    leftFoot = position;           // initial plant
    rightFoot = { position.x + 18, baseY }; // step forward
    fingers.resize(3);
}

void Walker::step() {
    time += 0.05f;

    // Walking logic
    if (walking) {
        walk_phase += 0.025f;
        if (walk_phase >= 1.0f) walk_phase -= 1.0f;
        // Swap stance leg every half cycle
        if (walk_phase < 0.5f)
            left_stance = true;
        else
            left_stance = false;
    }

    // Advance only if not at ball
    float to_ball = ballCenter.x - position.x;
    if (to_ball > 60 && walking) {
        position.x += speed;
    } else {
        walking = false;
        reached = true;
    }

    // Torso and hips
    updateTorso();

    // Legs (proper bipedal cycle)
    updateLegs();

    // Arms: swing during walk, reach when stopped
    updateArms();

    // Fingers: reach for ball when close
    if (reached)
        reachForBall();
    updateFingers();
}

void Walker::updateTorso() {
    // Bob up/down
    float bob = walking ? 6.0f * sinf(walk_phase * 2 * PI) : 0.0f;
    torsoBottom = { position.x, baseY - bob };
    torsoTop = { position.x, torsoBottom.y - torsoLen };
    headCenter = { torsoTop.x, torsoTop.y - headRadius - 4 };
}

void Walker::updateLegs() {
    // Parameters
    float upper = 46.0f, lower = 44.0f;

    // Hip offsets
    Vector2 hipL = { position.x - 10, torsoBottom.y };
    Vector2 hipR = { position.x + 10, torsoBottom.y };

    // Choose which foot is planted (stance) and which is swinging
    Vector2 stanceHip, swingHip;
    Vector2* stanceFootPtr;
    Vector2* swingFootPtr;
    bool leftIsStance = left_stance;
    if (leftIsStance) {
        stanceHip = hipL; swingHip = hipR;
        stanceFootPtr = &leftFoot; swingFootPtr = &rightFoot;
    } else {
        stanceHip = hipR; swingHip = hipL;
        stanceFootPtr = &rightFoot; swingFootPtr = &leftFoot;
    }

    // Stance foot stays fixed in x during its phase
    if (walking) {
        // Parameters for the foot arc
        float stepLength = 40.0f;
        float liftHeight = footLift;
        float phase = fmod(walk_phase * 2, 1.0f);

        // When phase advances to 0, swap legs (plant swing, swing stance)
        if (phase < 0.5f) {
            // Swing foot moves from behind to ahead in an arc
            float arc = phase / 0.5f;
            float sx = position.x + (leftIsStance ? 12 : -12)
                     + stepLength * (arc - 0.5f);
            float sy = baseY - liftHeight * sinf(PI * arc);
            *swingFootPtr = { sx, sy };
            // Stance foot remains where planted
            (*stanceFootPtr).y = baseY;
        } else {
            // Next half: switch roles
            float arc = (phase - 0.5f) / 0.5f;
            float sx = position.x + (leftIsStance ? -12 : 12)
                     - stepLength * (arc - 0.5f);
            float sy = baseY - liftHeight * sinf(PI * arc);
            *stanceFootPtr = { sx, sy };
            // Swing foot remains where planted
            (*swingFootPtr).y = baseY;
        }
    }

    // Solve for knees (2-segment IK with knee hint)
    auto solve_leg = [](Vector2 hip, Vector2 foot, float upper, float lower, Limb2& limb, float kneeHintSign) {
        Vector2 d = { foot.x - hip.x, foot.y - hip.y };
        float len = sqrtf(d.x * d.x + d.y * d.y);
        len = fminf(len, upper + lower - 1.0f);

        // Law of cosines for angles
        float a = acosf(fmaxf(-1.0f, fminf(1.0f, (upper*upper + len*len - lower*lower) / (2*upper*len))));
        float b = atan2f(d.x, d.y);
        // Knee hint: always bend "forward"
        float theta1 = b - kneeHintSign * fabsf(a);
        float theta2 = kneeHintSign * (PI - acosf(fmaxf(-1.0f, fminf(1.0f, (upper*upper + lower*lower - len*len) / (2*upper*lower)))));

        limb.root = hip;
        limb.len1 = upper;
        limb.len2 = lower;
        limb.theta1 = theta1;
        limb.theta2 = theta2;
        limb.joint = {
            hip.x + upper * sinf(theta1),
            hip.y + upper * cosf(theta1)
        };
        limb.tip = foot;
    };

    // Solve for each leg: knee always bends "forward" (downward)
    solve_leg(hipL, leftFoot, upper, lower, leftLeg, 1.0f);
    solve_leg(hipR, rightFoot, upper, lower, rightLeg, 1.0f);
}


void Walker::updateArms() {
    // Shoulders
    Vector2 shoulderL = { torsoTop.x - 18, torsoTop.y + 5 };
    Vector2 shoulderR = { torsoTop.x + 18, torsoTop.y + 5 };

    float upper = 38.0f, lower = 34.0f;
    // Swing arms during walk
    float phase = walk_phase * 2 * PI;
    float swingL = 0.6f * sinf(phase + PI); // out of phase with opposite leg
    float swingR = 0.6f * sinf(phase);

    // Default: swing arms
    leftArm.root = shoulderL;
    leftArm.len1 = upper;
    leftArm.len2 = lower;
    leftArm.theta1 = -1.2f + swingL;
    leftArm.theta2 = 0.4f * sinf(phase + 0.5f);

    leftArm.joint = {
        leftArm.root.x + upper * sinf(leftArm.theta1),
        leftArm.root.y + upper * cosf(leftArm.theta1)
    };
    leftArm.tip = {
        leftArm.joint.x + lower * sinf(leftArm.theta1 + leftArm.theta2),
        leftArm.joint.y + lower * cosf(leftArm.theta1 + leftArm.theta2)
    };

    rightArm.root = shoulderR;
    rightArm.len1 = upper;
    rightArm.len2 = lower;
    rightArm.theta1 = 1.2f + swingR;
    rightArm.theta2 = 0.4f * sinf(phase + PI + 0.5f);

    rightArm.joint = {
        rightArm.root.x + upper * sinf(rightArm.theta1),
        rightArm.root.y + upper * cosf(rightArm.theta1)
    };
    rightArm.tip = {
        rightArm.joint.x + lower * sinf(rightArm.theta1 + rightArm.theta2),
        rightArm.joint.y + lower * cosf(rightArm.theta1 + rightArm.theta2)
    };

    // When at ball, right arm reaches for it
    if (!walking) {
        // Use IK for right arm only (can do both if you wish)
        Vector2 target = ballCenter;
        float l1 = upper, l2 = lower;
        Vector2 d = { target.x - rightArm.root.x, target.y - rightArm.root.y };
        float len = sqrtf(d.x * d.x + d.y * d.y);
        len = fminf(len, l1 + l2 - 1.0f);

        float a = acosf(fmaxf(-1.0f, fminf(1.0f, (l1*l1 + len*len - l2*l2) / (2*l1*len))));
        float b = atan2f(d.x, d.y);
        rightArm.theta1 = b - a;
        rightArm.theta2 = PI - acosf(fmaxf(-1.0f, fminf(1.0f, (l1*l1 + l2*l2 - len*len) / (2*l1*l2))));

        rightArm.joint = {
            rightArm.root.x + l1 * sinf(rightArm.theta1),
            rightArm.root.y + l1 * cosf(rightArm.theta1)
        };
        rightArm.tip = {
            rightArm.joint.x + l2 * sinf(rightArm.theta1 + rightArm.theta2),
            rightArm.joint.y + l2 * cosf(rightArm.theta1 + rightArm.theta2)
        };
    }
}

void Walker::updateFingers() {
    // Only at ball: fingers on right hand
    if (walking) {
        for (int i = 0; i < 3; ++i) {
            fingers[i].root = rightArm.tip;
            fingers[i].len = 10;
            fingers[i].angle = rightArm.theta1 + rightArm.theta2 + (i - 1) * 0.28f;
            fingers[i].tip = {
                fingers[i].root.x + fingers[i].len * sinf(fingers[i].angle),
                fingers[i].root.y + fingers[i].len * cosf(fingers[i].angle)
            };
        }
    } else {
        // Fan out to cover ball
        for (int i = 0; i < 3; ++i) {
            fingers[i].root = rightArm.tip;
            fingers[i].len = 10;
            float angleToBall = atan2f(ballCenter.x - rightArm.tip.x, ballCenter.y - rightArm.tip.y);
            fingers[i].angle = angleToBall + (i - 1) * 0.5f;
            fingers[i].tip = {
                fingers[i].root.x + fingers[i].len * sinf(fingers[i].angle),
                fingers[i].root.y + fingers[i].len * cosf(fingers[i].angle)
            };
        }
    }
}

void Walker::reachForBall() {
    // All handled in updateArms() and updateFingers()
}

bool Walker::fingersTouchingBall() const {
    int count = 0;
    for (const auto& f : fingers)
        if (CheckCollisionPointCircle(f.tip, ballCenter, ballRadius + 1.0f))
            count++;
    return count >= 3;
}

void Walker::draw() {
    // Draw legs
    DrawLineV(leftLeg.root, leftLeg.joint, DARKBLUE);
    DrawLineV(leftLeg.joint, leftLeg.tip, DARKBLUE);
    DrawCircleV(leftLeg.tip, 3, RED);
    DrawLineV(rightLeg.root, rightLeg.joint, DARKBLUE);
    DrawLineV(rightLeg.joint, rightLeg.tip, DARKBLUE);
    DrawCircleV(rightLeg.tip, 3, RED);

    // Draw torso
    DrawLineV(torsoBottom, torsoTop, BLACK);

    // Draw arms
    DrawLineV(leftArm.root, leftArm.joint, DARKBLUE);
    DrawLineV(leftArm.joint, leftArm.tip, DARKBLUE);
    DrawCircleV(leftArm.tip, 3, RED);
    DrawLineV(rightArm.root, rightArm.joint, DARKBLUE);
    DrawLineV(rightArm.joint, rightArm.tip, DARKBLUE);
    DrawCircleV(rightArm.tip, 3, RED);

    // Draw head
    DrawCircleV(headCenter, headRadius, LIGHTGRAY);
    DrawCircleLines((int)headCenter.x, (int)headCenter.y, (int)headRadius, BLACK);

    // Draw fingers
    for (auto& f : fingers)
        DrawLineV(f.root, f.tip, MAROON), DrawCircleV(f.tip, 2, RED);

    // Draw the ball
    DrawCircleLines((int)ballCenter.x, (int)ballCenter.y, (int)ballRadius, RED);

    // Optional: Show text when all fingers touch
    if (fingersTouchingBall())
        DrawText("TOUCH!", (int)ballCenter.x + 25, (int)ballCenter.y - 30, 20, GREEN);
}

#include "Walker.h"
#include <algorithm>
#include <cmath>

Walker::Walker(Vector2 ballC, float ballR)
    : ballCenter(ballC), ballRadius(ballR) {}

void Walker::init() {
    t = 0.0f;
    reached = false;

    baseY = 500.0f;
    float legLength = 46.0f + 44.0f;
    position = { 100, baseY - legLength + 18.0f }; // Hips above ground

    speed = 2.0f;
    stride = 22.0f;   // reduced stride
    lift = 8.0f;      // reduced lift
    torsoLen = 80.0f;
    headRadius = 18.0f;
    stepPeriod = 48.0f; // larger = slower

    fingers.resize(3);
}
    


void Walker::step() {
    if (!reached && position.x < ballCenter.x - 65) {
        position.x += speed;
        t += 2 * PI / stepPeriod;
    } else {
        reached = true;
    }


    updateTorso();
    updateLegs();
    updateArms();
    // if (reached) reachForBall();
    updateFingers();
}

void Walker::updateTorso() {
    float bob = 3.0f * sinf(2 * t);
    float sway = 3.0f * sinf(t);

    float legLength = 46.0f + 44.0f;
    float hipY = baseY - legLength + 18.0f - bob;
    torsoBottom = { position.x + sway, hipY };

    // Torso lean angle
    float torsoAngle = 0.0f;
    float squatAmount = 0.0f;

    if (reached) {
        float upperArm = 38.0f, lowerArm = 34.0f, finger = 10.0f;
        float maxReach = upperArm + lowerArm + finger;

        Vector2 testTorsoTop = { torsoBottom.x, torsoBottom.y - torsoLen };
        float dist = sqrtf((testTorsoTop.x - ballCenter.x) * (testTorsoTop.x - ballCenter.x) +
                           (testTorsoTop.y - ballCenter.y) * (testTorsoTop.y - ballCenter.y));
        if (dist > maxReach) {
            float neededReach = dist - maxReach;
            float maxSquat = 38.0f; // max hip drop (tune for your model)
            squatAmount = std::min(neededReach, maxSquat);
            torsoBottom.y += squatAmount;

            // Recompute after squat
            testTorsoTop = { torsoBottom.x, torsoBottom.y - torsoLen };
            dist = sqrtf((testTorsoTop.x - ballCenter.x) * (testTorsoTop.x - ballCenter.x) +
                         (testTorsoTop.y - ballCenter.y) * (testTorsoTop.y - ballCenter.y));
            if (dist > maxReach) {
                // If still can't reach, lean the torso
                Vector2 toBall = { ballCenter.x - torsoBottom.x, ballCenter.y - torsoBottom.y };
                torsoAngle = std::clamp(atan2f(toBall.x, toBall.y), -0.7f, 0.7f);
            }
        }
    }

    torsoTop = {
        torsoBottom.x + torsoLen * sinf(torsoAngle),
        torsoBottom.y - torsoLen * cosf(torsoAngle)
    };
    headCenter = { torsoTop.x, torsoTop.y - headRadius - 4 };
}

void Walker::updateLegs() {
    float upper = 46.0f, lower = 44.0f;

    float cycle = fmod(t, 2 * PI);

    float phaseL = cycle;
    float phaseR = cycle + PI;

    // Parameters for a more human walk
    float stanceDuration = 0.6f;
    float swingStart = 2 * PI * (1 - stanceDuration);

    // WIDER hips and feet for a bigger stride
    auto compute_leg = [&](float phase, Vector2 hip, bool isLeft) -> std::pair<Vector2, float> {
        float normPhase = fmod(phase, 2 * PI);

        float stepX = 0, stepY = 0, kneeBend = 0;
        if (normPhase < swingStart) {
            // SWING
            float swingNorm = normPhase / swingStart;
            stepX = (isLeft ? -13.0f : 13.0f) + stride * (2 * swingNorm - 1); // widened foot offset
            stepY = baseY - lift * sinf(PI * swingNorm);
            kneeBend = 0.9f * (1 - swingNorm);
        } else {
            // STANCE
            stepX = (isLeft ? -13.0f : 13.0f); // widened foot offset
            stepY = baseY;
            kneeBend = 0.2f * (1 - cosf(PI * (normPhase - swingStart) / (2 * PI * stanceDuration)));
        }
        Vector2 foot = { position.x + stepX, stepY };
        return { foot, kneeBend };
    };

    // WIDENED hips for natural gait
    Vector2 hipL = { torsoBottom.x - 12, torsoBottom.y };
    Vector2 hipR = { torsoBottom.x + 12, torsoBottom.y };

    auto [footL, bendL] = compute_leg(phaseL, hipL, true);
    auto [footR, bendR] = compute_leg(phaseR, hipR, false);

    // Solve 2-bone IK, blend the knee bend in swing phase
    auto solve_leg = [](Vector2 hip, Vector2 foot, float upper, float lower, float kneeBlend, Limb2& limb) {
        Vector2 d = { foot.x - hip.x, foot.y - hip.y };
        float len = sqrtf(d.x * d.x + d.y * d.y);
        len = fminf(len, upper + lower - 1.0f);

        float a_straight = 0.0f;
        float a_bent = acosf(fmaxf(-1.0f, fminf(1.0f, (upper*upper + len*len - lower*lower) / (2*upper*len))));
        float a = kneeBlend * a_bent + (1 - kneeBlend) * a_straight;

        float b = atan2f(d.x, d.y);
        float theta1 = b + fabsf(a);
        float theta2 = PI - acosf(fmaxf(-1.0f, fminf(1.0f, (upper*upper + lower*lower - len*len) / (2*upper*lower))));
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

    solve_leg(hipL, footL, upper, lower, bendL, leftLeg);
    solve_leg(hipR, footR, upper, lower, bendR, rightLeg);
}

void Walker::updateArms() {
    Vector2 shoulderL = { torsoTop.x - 18, torsoTop.y + 5 };
    Vector2 shoulderR = { torsoTop.x + 18, torsoTop.y + 5 };

    float upper = 38.0f, lower = 34.0f;
    float swingL = 0.8f * sinf(t + PI);
    float swingR = 0.8f * sinf(t);

    leftArm.root = shoulderL;
    leftArm.len1 = upper;
    leftArm.len2 = lower;
    leftArm.theta1 = -1.2f + swingL;
    leftArm.theta2 = 0.4f * sinf(t + 0.5f);
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
    rightArm.theta2 = 0.4f * sinf(t + PI + 0.5f);
    rightArm.joint = {
        rightArm.root.x + upper * sinf(rightArm.theta1),
        rightArm.root.y + upper * cosf(rightArm.theta1)
    };
    rightArm.tip = {
        rightArm.joint.x + lower * sinf(rightArm.theta1 + rightArm.theta2),
        rightArm.joint.y + lower * cosf(rightArm.theta1 + rightArm.theta2)
    };

    if (reached) {
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
    for (int i = 0; i < 3; ++i) {
        fingers[i].root = rightArm.tip;
        fingers[i].len = 10;
        float spread = reached ? 0.5f : 0.28f;
        fingers[i].angle = rightArm.theta1 + rightArm.theta2 + (i - 1) * spread;
        fingers[i].tip = {
            fingers[i].root.x + fingers[i].len * sinf(fingers[i].angle),
            fingers[i].root.y + fingers[i].len * cosf(fingers[i].angle)
        };
    }
}

// void Walker::reachForBall() {
//     // Already handled in updateArms() and updateFingers()
// }

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

    // Show text when all fingers touch
    if (fingersTouchingBall())
        DrawText("TOUCH!", (int)ballCenter.x + 25, (int)ballCenter.y - 30, 20, GREEN);
}

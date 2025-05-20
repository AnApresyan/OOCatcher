#pragma once
#include "raylib.h"
#include "OOCatcher.h"
#include <vector>

struct Limb2 {
    Vector2 root;
    float len1, len2;
    float theta1, theta2;
    Vector2 joint, tip;
};

struct Finger2 {
    Vector2 root;
    float len, angle;
    Vector2 tip;
};

class Walker : public OOCatcher {
public:
    Walker(Vector2 ballCenter, float ballRadius);
    void init() override;
    void step() override;
    void draw() override;
    void setStandUp(bool value);
    bool fingersTouchingBall() const;
    Vector2 getHandPos() const;
    void setThrowAnim(float windup, float throwFwd);

private:
    float t; // phase
    Vector2 position;
    float torsoLen, headRadius, baseY, speed, stride, lift, stepPeriod;
    Limb2 leftLeg, rightLeg, leftArm, rightArm;
    std::vector<Finger2> fingers;
    Vector2 torsoTop, torsoBottom, headCenter;
    Vector2 ballCenter;
    float ballRadius;
    bool reached;
    void updateTorso();
    void updateLegs();
    void updateArms();
    void updateFingers();
    // void reachForBall();
    bool standUp = false;
    float throwWindupPhase = 0.0f;
    float throwFwdPhase = 0.0f;
};

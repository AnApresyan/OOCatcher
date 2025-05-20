#pragma once
#include <vector>
#include "BodySegment.h"
class OOCatcher
{
public:
    std::vector<BodySegment *> segments;
    virtual void init() = 0;
    virtual void step() = 0;
    virtual void draw() = 0;
    virtual ~OOCatcher();
};
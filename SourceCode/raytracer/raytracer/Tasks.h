#pragma once
#include "Intersection.h"

struct ShaderTask {
    IntersectionData intersectionData {};
    int pixelX = 0;
    int pixelY = 0;
    uint32_t depth = 0;

    ShaderTask(IntersectionData intersectionData, int pixelX, int pixelY)
        : intersectionData(intersectionData), pixelX(pixelX), pixelY(pixelY) {}
};


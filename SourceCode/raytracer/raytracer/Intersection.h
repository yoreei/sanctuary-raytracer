#pragma once
#include <string>
#include "CRTTypes.h"

enum class Intersection {
    SUCCESS,
    PARALLEL,
    BACKFACE,
    OUT_OF_BOUNDS,
    BEHIND_RAY_ORIGIN
};

std::string toString(Intersection intersection);

struct IntersectionData {
    float t = FLT_MAX;    // Distance
    Vec3 p = { 0, 0, 0 };     // Intersection Point
    Vec3 n = { 0, 0, 0 };     // Normal at Point
    float u = 0.f;    // First Barycentric Base
    float v = 0.f;    // Second Barycentric Base
    size_t materialIndex = 0;

    bool intersectionSuccessful() const { // TODO remove this
        return t < FLT_MAX;
    }
};


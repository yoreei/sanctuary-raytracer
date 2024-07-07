#pragma once
#include <string>
#include "CRTTypes.h"

enum class IntersectionType {
    SUCCESS,
    PARALLEL,
    BACKFACE,
    OUT_OF_BOUNDS,
    BEHIND_RAY_ORIGIN
};

std::string toString(IntersectionType intersection);

struct Intersection {
    float t = FLT_MAX;    // Distance
    Vec3 p = { 0, 0, 0 };     // Intersection Point
    Vec3 n = { 0, 0, 0 };     // Normal at Point
    float u = 0.f;    // First Barycentric Base
    float v = 0.f;    // Second Barycentric Base
    size_t materialIndex = 0;
    IntersectionType type = IntersectionType::OUT_OF_BOUNDS;
    bool successful() const { return type == IntersectionType::SUCCESS; }
};


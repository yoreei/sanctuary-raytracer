#pragma once
#include <string>

enum class Intersection {
    SUCCESS,
    PARALLEL,
    BACKFACE,
    OUT_OF_BOUNDS,
    BEHIND_RAY_ORIGIN
};

std::string toString(Intersection intersection);

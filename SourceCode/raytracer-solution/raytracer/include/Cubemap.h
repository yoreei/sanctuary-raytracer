#pragma once
#include <algorithm>
#include <array>
#include <cmath>

#include "include/Image.h"

class Cubemap
{
public:
    Cubemap() = default;

    /* front, right, back, left, up, down */
    std::array<Image, 6> images {};
    Vec3 sample(const Vec3& dir) const;

    /* Sample the cubemap and calculate the average color */
    Vec3 calculateAmbientColor();

private:
    /* @brief: Sampling based on golden ratio */
    static std::vector<Vec3> fibonacciSphereSample(int sampleCount);
};


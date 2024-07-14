#pragma once
#include <cstdint>
#include <algorithm>

namespace CRTMath
{
    inline float max(float a, float b, float c) {
        return std::max(a, std::max(b, c));
    }

    /*
    * log10 approximation
    */
    inline float fastLog(float x) {
        union { float f; uint32_t i; } vx = { x };
        union { uint32_t i; float f; } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
        float y = float(vx.i);
        y *= 1.1920928955078125e-7f;

        return y - 124.225514990f
               - 1.498030302f * mx.f
               - 1.725879990f / (0.3520887068f + mx.f);
    }

};


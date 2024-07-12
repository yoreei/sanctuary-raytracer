#pragma once

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>

#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion failed: (" #condition "), function " << __FUNCTION__ \
                      << ", file " << __FILE__ << ", line " << __LINE__ << "." << std::endl; \
            std::abort(); \
        } \
    } while (false)

template<class T>
inline bool contains(const std::vector<T>& vec, const T& item) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
}

inline std::string stringFromBool(bool b) {
    return b ? "true" : "false";
}

inline constexpr float epsilon = 1e-5f; // high precision
// inline constexpr float epsilon = 1e-4f; // good precision

namespace Utils {
    inline float max(float a, float b, float c) {
        return std::max(a, std::max(b, c));
    }

    inline float fastLog(float x) {
        union { float f; uint32_t i; } vx = { x };
        union { uint32_t i; float f; } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
        float y = float(vx.i);
        y *= 1.1920928955078125e-7f;

        return y - 124.225514990f
               - 1.498030302f * mx.f
               - 1.725879990f / (0.3520887068f + mx.f);
    }
} // namespace Utils

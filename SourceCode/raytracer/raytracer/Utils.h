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


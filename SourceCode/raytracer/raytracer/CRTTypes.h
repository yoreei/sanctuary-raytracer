#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <numbers>
#include <iostream>
#include <string>
#include <cassert>
#include <array>

#include "Utils.h"
#include "json_fwd.h"

inline bool fEqual(float a, float b, float maxDiff = epsilon) {
    return std::abs(a - b) < maxDiff;
}

inline bool fLower(float a, float b, float maxDiff = epsilon) {
    return a - b < -maxDiff;
}

class Vec3 {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;

    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    float& axis(size_t i) {
        if (i == 0) return x; 
        if (i == 1) return y; 
        if (i == 2) return z; 
        throw std::runtime_error("Vec3::axis: bad parameter"); 
    }

    const float& axis(size_t i) const {
        if (i == 0) return x; 
        if (i == 1) return y; 
        if (i == 2) return z; 
        throw std::runtime_error("Vec3::axis: bad parameter"); 
    }

    nlohmann::ordered_json toJson() const;

    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    bool equal(const Vec3& other, float maxDiff = epsilon) const {
        return  fEqual(this->x, other.x, maxDiff) &&
            fEqual(this->y, other.y, maxDiff) &&
            fEqual(this->z, other.z, maxDiff);
    }

    Vec3 operator/(float scalar) const {
        float divCache = 1.f / scalar; // division optimization
        return Vec3(x * divCache, y * divCache, z * divCache);
    }

    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    void cross(const Vec3& other, Vec3& out) const {
        out.x = y * other.z - z * other.y;
        out.y = z * other.x - x * other.z;
        out.z = x * other.y - y * other.x;
    }

    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float lengthSquared() const {
        return x * x + y * y + z * z;
    }

    float crossLength(const Vec3& other) const {
        float cx = y * other.z - z * other.y;
        float cy = z * other.x - x * other.z;
        float cz = x * other.y - y * other.x;
        return std::sqrt(cx * cx + cy * cy + cz * cz);
    }

    [[nodiscard]] Vec3 getUnit() const {
        Vec3 result = *this;
        result.normalize();
        return result;
    }

    bool isUnit() const {
        return getUnit().equal(*this);
    }

    void normalize() {
        float len = length();
        float divCache = 1.f / len; // division optimization
        x = x * divCache;
        y = y * divCache;
        z = z * divCache;
    }

    std::string toString() const
    {
        return "{ " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + " }";
    }

    void clamp(float min, float max) {
        x = std::clamp(x, min, max);
        y = std::clamp(y, min, max);
        z = std::clamp(z, min, max);
    }

    static Vec3 MakeLowest() {
        return Vec3{ std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };
    }

    static Vec3 MakeMax() {
        return Vec3{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
    }

    static void componentMax(const Vec3& vec0, const Vec3& vec1, Vec3& vec2Output);

    static void componentMin(const Vec3& vec0, const Vec3& vec1, Vec3& vec2Output);

};

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    Vec3 result;
    a.cross(b, result);
    return result;
}

inline float dot(const Vec3& a, const Vec3& b) {
    return a.dot(b);
}

inline Vec3 operator*(float scalar, const Vec3& vec) {
    return vec * scalar;
}

/**
 * Multiply two vectors component-wise.
 */
inline Vec3 multiply(const Vec3& a, const Vec3& b) {
    return { a.x * b.x, a.y * b.y, a.z * b.z };
}

/**
* @brief Linear interpolation between two vectors.
* @param t: weight of the second vector.
*/
inline Vec3 lerp(const Vec3& a, const Vec3& b, float t) {
    return {
        a.x * (1 - t) + b.x * t,
        a.y * (1 - t) + b.y * t,
        a.z * (1 - t) + b.z * t
    };
}

inline Vec3 slerp(const Vec3& vec0, const Vec3& vec1, float t) {
    assert(vec0.isUnit());
    assert(vec1.isUnit());

    float dot = vec0.dot(vec1);
    dot = std::clamp(dot, -1.f, 1.f);
    float theta = std::acos(dot);

    // Special case which preserves numerical stability
    if (std::fabs(theta) < epsilon) {
        return lerp(vec0, vec1, t);
    }

    float sinTheta = std::sin(theta);

    float factor0 = std::sin((1 - t) * theta) / sinTheta;
    float factor1 = std::sin(t * theta) / sinTheta;

    return vec0 * factor0 + vec1 * factor1;
}

inline std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
    os << vec.toString();
    return os;
}

/*
* Linear Algebra Square Matrix in R^3
* Cost: 9 floats
*
* Example usage:
* auto mat = Matrix3x3.identity()
* mat(col, row) = 1.0f; // Column-major data access.
*/

class Matrix3x3 {
public:
    Matrix3x3() {
        for (size_t i = 0; i < 9; ++i) {
            data[i] = (i % 4 == 0) ? 1.0f : 0.0f; // Identity matrix
        }
    }

    Matrix3x3(const std::vector<float>& values) {
        if (values.size() != 9) {
            throw std::invalid_argument("Matrix3x3 requires 9 values.");
        }

        std::copy(values.begin(), values.end(), data.begin());
    }

    static Matrix3x3 fromCols(const Vec3& c0, const Vec3& c1, const Vec3& c2) {
        return Matrix3x3{ {
            c0.x, c1.x, c2.x,
            c0.y, c1.y, c2.y,
            c0.z, c1.z, c2.z,
            } };
    }

    float& operator()(size_t row, size_t col) {
        return data[row * 3 + col];
    }

    const float& operator()(size_t row, size_t col) const {
        return data[row * 3 + col];
    }

    nlohmann::ordered_json toJson() const;

    Matrix3x3 operator*(const Matrix3x3& other) const {
        Matrix3x3 result;
        for (size_t row = 0; row < 3; ++row) {
            for (size_t col = 0; col < 3; ++col) {
                result(row, col) = 0.0f;
                for (size_t k = 0; k < 3; ++k) {
                    result(row, col) += (*this)(row, k) * other(k, col);
                }
            }
        }
        return result;
    }

    Matrix3x3 operator+(const Matrix3x3& other) const {
        return {{
                (*this)(0,0) + other(0,0), (*this)(0, 1) + other(0,1), (*this)(0,2) + other(0,2),
                (*this)(1,0) + other(1,0), (*this)(1, 1) + other(1,1), (*this)(1,2) + other(1,2),
                (*this)(2,0) + other(2,0), (*this)(2, 1) + other(2,1), (*this)(2,2) + other(2,2),

            }};
    }

    Vec3 operator*(const Vec3& vec) const {
        Vec3 result;
        result.x = (*this)(0, 0) * vec.x + (*this)(0, 1) * vec.y + (*this)(0, 2) * vec.z;
        result.y = (*this)(1, 0) * vec.x + (*this)(1, 1) * vec.y + (*this)(1, 2) * vec.z;
        result.z = (*this)(2, 0) * vec.x + (*this)(2, 1) * vec.y + (*this)(2, 2) * vec.z;
        return result;
    }

    Matrix3x3 operator*(const float num) const {
        return {{
                (*this)(0,0) * num, (*this)(0, 1) * num, (*this)(0, 2) * num,
                (*this)(1,0) * num, (*this)(1, 1) * num, (*this)(1, 2) * num,
                (*this)(2,0) * num, (*this)(2, 1) * num, (*this)(2, 2) * num,

            }};
    }

    Matrix3x3& operator*=(const Matrix3x3& other) {
        *this = *this * other;
        return *this;
    }

    Vec3 getCol(size_t num) const
    {
        if (num > 2) {
            throw std::out_of_range("Matrix3x3::getCol: Index out of range: " + std::to_string(num));
        }
        return Vec3{ data[num], data[num + size_t(3)], data[num + size_t(6)] };
    }

    void setCol(size_t num, const Vec3& vec)
    {
        if (num > 2) {
            throw std::out_of_range("Matrix3x3::setCol: Index out of range: " + std::to_string(num));
        }
        data[num] = vec.x;
        data[num + size_t(3)] = vec.y;
        data[num + size_t(6)] = vec.z;
    }

    Vec3 row(uint8_t num) const
    {
        if (num > 2) {
            throw std::out_of_range("Matrix3x3::row: Index out of range: " + std::to_string(num));
        }
        return Vec3{ data[num * 3], data[num * 3 + 1], data[num * 3 + 2] };
     }

    static Matrix3x3 identity() {
        return Matrix3x3();
    }

    static Matrix3x3 translation(float tx, float ty) {
        Matrix3x3 result = identity();
        result(0, 2) = tx;
        result(1, 2) = ty;
        return result;
    }

    static Matrix3x3 scaling(float sx, float sy) {
        Matrix3x3 result = identity();
        result(0, 0) = sx;
        result(1, 1) = sy;
        return result;
    }

    static Matrix3x3 rotation2D(float angle) {
        Matrix3x3 result = identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        result(0, 0) = c;
        result(0, 1) = -s;
        result(1, 0) = s;
        result(1, 1) = c;
        return result;
    }

    /* Rotate around X. pitch. Cinema 'tilt' */
    static Matrix3x3 Pitch(float deg) {
        float rad = radFromDeg * deg;
        float cosv = cos(rad);
        float sinv = sin(rad);

        return Matrix3x3{ {
        1.f, 0.f, 0.f,
        0.f, cosv, -sinv,
        0.f, sinv, cosv
        } };
    }

    /* Rotate around Y. yaw. Cinema 'pan' */
    static Matrix3x3 Yaw(float deg) {
        float rad = radFromDeg * deg;
        float cosv = cos(rad);
        float sinv = sin(rad);

        return Matrix3x3{ {
        cosv,  0, sinv,
        0,     1, 0,
        -sinv, 0, cosv
        } };
    }

    /*
    * Rotate around Z
    */
    static Matrix3x3 Roll(float deg) {
        float rad = radFromDeg * deg;
        float cosv = cos(rad);
        float sinv = sin(rad);

        return Matrix3x3{ {
        cosv, -sinv, 0,
        sinv, cosv, 0,
        0,0, 1
        } };
    }

    bool isOrthonormal() const {
        Vec3 col0 = getCol(0);
        Vec3 col1 = getCol(1);
        Vec3 col2 = getCol(2);
        
        bool unitVectors = col0.isUnit() &&
                           col1.isUnit() && 
                           col2.isUnit();

        bool perpendicular = fEqual(col0.dot(col1), 0.0f) &&
                             fEqual(col0.dot(col2), 0.0f) &&
                             fEqual(col1.dot(col2), 0.0f);

        return unitVectors && perpendicular;
    }

    std::string toString() const {
        std::string result = "";
        for (size_t row = 0; row < 3; ++row) {
            for (size_t col = 0; col < 3; ++col) {
                result += std::to_string((*this)(row, col));
                result += " ";
            }
            result += "\n";
        }
        return result;
    }

private:
    std::array<float, 9> data;
    static constexpr float radFromDeg = static_cast<float>(std::numbers::pi) / 180.0f;
};

class Ray {
public:
    Vec3 origin;
    Vec3 direction;
    Vec3 invdir;
    int sign[3];

    Ray(const Vec3& origin, const Vec3& direction) : origin(origin), direction(direction)
    {
        invdir = {1 / direction.x, 1 / direction.y, 1 / direction.z};
        sign[0] = (invdir.x < 0);
        sign[1] = (invdir.y < 0);
        sign[2] = (invdir.z < 0);
    }

    /*
    * @param N: expected to face the ray.
    */
    void reflect(const Vec3& point, const Vec3& N) {
        float cosi = dot(direction, N);
        origin = point;
        direction = direction - 2 * cosi * N;
        direction.normalize();
    }

    bool compareRefract(const Vec3 point, const Vec3& N, float etai, float etat);
    /*
    * algorithm from Scratchapixel
    * @param N: expected to face the ray.
    */
    bool refractSP(const Vec3 point, const Vec3& N, float etai, float etat);

    /**
    * @brief: Inuitive implementation, suffers from precision issues.
    * @return false if no refraction (i.e. total internal reflection)
    * @param normal: expected to face the ray.
    **/
    bool refractVladi(const Vec3& point, Vec3 normal, float iorI, float iorR);
};


struct TraceTask {
    Ray ray;
    size_t pixelX{ 0 };
    size_t pixelY{ 0 };
    uint32_t depth{ 0 };
    // [0, 1]. Higher values increase the weight of the diffuseComponent of the hit point when `lerp`ing.  See ImageQueue::flatten
    float weight{ 1.f };
    float ior{ 1.f };
};


struct Color
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    std::string toString() const
    {
        return std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b) + "\t";
    }

    static const uint8_t maxColorComponent = 255;

    /* Create Color from RGB values in the range [0, 1] */
    static Color fromUnit(float fr, float fg, float fb) {
        auto r = static_cast<uint8_t>(std::round(fr * maxColorComponent));
        auto g = static_cast<uint8_t>(std::round(fg * maxColorComponent));
        auto b = static_cast<uint8_t>(std::round(fb * maxColorComponent));
        CHECK(r <= maxColorComponent && g <= maxColorComponent && b <= maxColorComponent);
        return Color{ r, g, b };
    }

    static Color fromUnit(const Vec3& vec) {
        return fromUnit(vec.x, vec.y, vec.z);
    }

};

inline std::ostream& operator<<(std::ostream& os, const Color& color) {
    os << color.toString();
    return os;
}


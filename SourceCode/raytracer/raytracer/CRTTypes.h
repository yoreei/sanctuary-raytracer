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

#include "Utils.h"

inline bool fequal(float a, float b, float epsilon = 0.0001f) {
    return std::abs(a - b) < epsilon;

}

inline bool flower(float a, float b, float epsilon = 0.0001f) {
    return a - b < -epsilon;
}

class Vec3 {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;

    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

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

    bool equal(const Vec3& other, float epsilon = 0.0001f) const {
        return fequal(this->x, other.x, epsilon) && fequal(this->y, other.y, epsilon) && fequal(this->z, other.z, epsilon);
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
        for (int i = 0; i < 9; ++i) {
            data[i] = (i % 4 == 0) ? 1.0f : 0.0f; // Identity matrix
        }
    }

    Matrix3x3(const std::vector<float>& values) {
        if (values.size() != 9) {
            throw std::invalid_argument("Matrix3x3 requires 9 values.");
        }
        for (int i = 0; i < 9; ++i) {
            data[i] = values[i];
        }
    }

    static Matrix3x3 fromCols(const Vec3& c0, const Vec3& c1, const Vec3& c2) {
        return Matrix3x3{ {
            c0.x, c1.x, c2.x,
            c0.y, c1.y, c2.y,
            c0.z, c1.z, c2.z,
            } };
    }

    float& operator()(int row, int col) {
        return data[row * 3 + col];
    }

    const float& operator()(int row, int col) const {
        return data[row * 3 + col];
    }

    Matrix3x3 operator*(const Matrix3x3& other) const {
        Matrix3x3 result;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                result(row, col) = 0.0f;
                for (int k = 0; k < 3; ++k) {
                    result(row, col) += (*this)(row, k) * other(k, col);
                }
            }
        }
        return result;
    }

    Vec3 operator*(const Vec3& vec) const {
        Vec3 result;
        result.x = (*this)(0, 0) * vec.x + (*this)(0, 1) * vec.y + (*this)(0, 2) * vec.z;
        result.y = (*this)(1, 0) * vec.x + (*this)(1, 1) * vec.y + (*this)(1, 2) * vec.z;
        result.z = (*this)(2, 0) * vec.x + (*this)(2, 1) * vec.y + (*this)(2, 2) * vec.z;
        return result;
    }

    Matrix3x3& operator*=(const Matrix3x3& other) {
        *this = *this * other;
        return *this;
    }

    Vec3 col(uint8_t num) const
    {
        if (num > 2) {
            throw std::out_of_range("Matrix3x3::col: Index out of range: " + num);
        }
        return Vec3{ data[num], data[num + 3], data[num + 6] };
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

    /* Rotate around X. pitch */
    static Matrix3x3 Tilt(float deg) {
        float rad = radFromDeg * deg;
        float cosv = cos(rad);
        float sinv = sin(rad);

        return Matrix3x3{ {
        1.f, 0.f, 0.f,
        0.f, cosv, -sinv,
        0.f, sinv, cosv
        } };
    }

    /* Rotate around Y. yaw */
    static Matrix3x3 Pan(float deg) {
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



    void toString() const {
        std::string result = "";
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                result += std::to_string((*this)(row, col));
                result += " ";
            }
            result += "\n";
        }
    }

private:
    float data[9];
    static constexpr float radFromDeg = static_cast<float>(std::numbers::pi) / 180.0f;
};

struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray() : origin(Vec3()), direction(Vec3()) {}

    Ray(const Vec3& origin, const Vec3& direction) : origin(origin), direction(direction) {}

    void reflect(const Vec3& point, const Vec3& normal) {
        origin = point;
        direction = direction - 2 * dot(direction, normal) * normal;
        assert(fequal(direction.length(), 1.f));
    }

    /**
    * @param normal: expected to face the ray.
    **/
    void refract(const Vec3& point, Vec3 normal, float iorI, float iorR) {
        assert(dot(direction,normal) < -1e-6); // usage req: normal should face the ray

        auto dbgOldDirection = direction;
        origin = point;
        float cosI = -dot(direction, normal);
        float sinI = std::sqrt(1 - cosI * cosI);
        float sinR = sinI * iorI / iorR;
        float cosR = std::sqrt(1 - sinR * sinR);
        Vec3 C = (direction + cosI * normal).getUnit();
        Vec3 B = C * sinR;
        Vec3 A = -normal * cosR;
        direction = A + B;

        assert(fequal(direction.length(), 1.f));
        assert(fequal(dot(direction, normal), -cosR));
        // assert R not going backwards:
        assert(dot(direction, dbgOldDirection) > 1e-6);
        // assert Snell's Law:
        assert(fequal(sinI / sinR, iorR / iorI));
    }
};

struct TraceTask {
    Ray ray;
    int pixelX{ 0 };
    int pixelY{ 0 };
    uint32_t depth{ 0 };
    // [0, 1]. RGB
    Vec3 color{ 1.f, 1.f, 1.f };
    // [0, 1]. i.e. inverseColorWeight. Higher values diminish the weight of `color` when `lerp`ing. 
    float attenuation{ 1.f };
    float ior{ 1.f };

    TraceTask(const Ray& ray, int pixelX, int pixelY)
        : ray(ray), pixelX(pixelX), pixelY(pixelY) {}

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


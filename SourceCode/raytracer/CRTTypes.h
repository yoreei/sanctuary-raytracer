#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <numbers>
#include <iostream>
#include <string>

struct Color
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    std::string toString() const
    {
        return std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b) + "\t";
    }

    static const int maxColorComponent = 255;
};

struct Vec3 {
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    Vec3() = default;

    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    Vec3 operator/(float scalar) const {
        float divCache = 1.f/scalar; // division optimization
        return Vec3(x * divCache, y * divCache, z * divCache);
    }

    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    [[nodiscard]] Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    [[nodiscard]] Vec3 normalize() const {
        float len = length();
        float divCache = 1.f/len; // division optimization
        return Vec3(x * divCache, y * divCache, z * divCache);
    }

    std::string toString() const
    {
        return "{ " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + " }";
    }

};

inline std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
    os << vec.toString();
    return os;
}

inline float dot(const Vec3& a, const Vec3& b) {
    return a.dot(b);
}
inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return a.cross(b);
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
        return Matrix3x3 {{
            c0.x, c1.x, c2.x,
            c0.y, c1.y, c2.y,
            c0.z, c1.z, c2.z,
            }};
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

        return Matrix3x3 {{
        1.f, 0.f, 0.f,
        0.f, cosv, -sinv,
        0.f, sinv, cosv
        }};
    }

    /* Rotate around Y. yaw */
    static Matrix3x3 Pan(float deg) {
        float rad = radFromDeg * deg;
        float cosv = cos(rad);
        float sinv = sin(rad);

        return Matrix3x3 {{
        cosv, 0, sinv,
        0, 1, 0,
        -sinv, 0, cosv
        }};
    }

    /*
    * Rotate around Z
    */
    static Matrix3x3 Roll(float deg) {
        float rad = radFromDeg * deg;
        float cosv = cos(rad);
        float sinv = sin(rad);

        return Matrix3x3 {{
        cosv, -sinv, 0,
        sinv, cosv, 0,
        0,0, 1
        }};
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
};
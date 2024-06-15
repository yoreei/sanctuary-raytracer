#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>

struct Pixel
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    std::string toString() const
    {
        return std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b) + "\t";
    }
};

struct Buffer2D
{
    Buffer2D(uint16_t _width, uint16_t _height) : width(_width), height(_height)
    {
        data = std::make_unique<Pixel[]>(width * height);
    }

    std::unique_ptr<Pixel[]> data;

    const uint16_t width;
    const uint16_t height;
};

struct Vec3 {
    float x, y, z;

    Vec3() : x{.0f}, y{.0f}, z{.0f} {}

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

    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3 normalize() const {
        float len = length();
        float divCache = 1.f/len; // division optimization
        return Vec3(x * divCache, y * divCache, z * divCache);
    }

    std::string toString() const
    {
        return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);
    }
};

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

    static Matrix3x3 rotation(float angle) {
        Matrix3x3 result = identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        result(0, 0) = c;
        result(0, 1) = -s;
        result(1, 0) = s;
        result(1, 1) = c;
        return result;
    }

    void toString() const {
        std::string result = "";
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                result += (*this)(row, col);
                result += " ";
            }
            result += "\n";
        }
    }

private:
    float data[9];
};

// hw4 Task 1
class Triangle {
public:
    Vec3 v0, v1, v2;

    Triangle() : v0(Vec3()), v1(Vec3()), v2(Vec3()) {}

    Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2) : v0(v0), v1(v1), v2(v2) {}

    Vec3 normal() const {
        Vec3 edge1 = v1 - v0;
        Vec3 edge2 = v2 - v0;
        return edge1.cross(edge2).normalize();
    }

    float area() const {
        Vec3 edge1 = v1 - v0;
        Vec3 edge2 = v2 - v0;
        return edge1.cross(edge2).length() * 0.5f;
    }

    Triangle transform(const Matrix3x3& mat) const {
        Vec3 newV0 = mat * v0;
        Vec3 newV1 = mat * v1;
        Vec3 newV2 = mat * v2;
        return Triangle(newV0, newV1, newV2);
    }

    std::string toString() const {
        return "Triangle: {" + v0.toString() + ", " + v1.toString() + ", " + v2.toString() + "}";
    }
};

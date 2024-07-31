#pragma once
#include <array>
#include <cmath>
#include "Globals.h"
#include "Image.h"
#include <algorithm>

class Cubemap
{
public:
    Cubemap() = default;

    /* front, right, back, left, up, down */
    std::array<Image, 6> images {};
    Vec3 sample(const Vec3& dir) const {

        float absX = std::fabs(dir.x);
        float absY = std::fabs(dir.y);
        float absZ = std::fabs(dir.z);

        int faceIndex;
        float u, v;

        // Determine which face of the cubemap the direction vector hits
        if (absX >= absY && absX >= absZ) {
            if (dir.x > 0) { 
                faceIndex = 1; // right
                u = -dir.z / absX;
                v = -dir.y / absX;
            } else { 
                faceIndex = 3; // left
                u = dir.z / absX;
                v = -dir.y / absX;
            }
        } else if (absY >= absX && absY >= absZ) {
            if (dir.y > 0) { 
                faceIndex = 4; // up
                u = dir.x / absY;
                v = dir.z / absY;
            } else { 
                faceIndex = 5; // down
                u = dir.x / absY;
                v = -dir.z / absY;
            }
        } else {
            if (dir.z > 0) { 
                faceIndex = 0; // front
                u = dir.x / absZ;
                v = -dir.y / absZ;
            } else { 
                faceIndex = 2; // back
                u = -dir.x / absZ;
                v = -dir.y / absZ;
            }
        }

        // Normalize u, v coordinates from [-1, 1] to [0, 1]
        u = (u + 1.0f) * 0.5f;
        v = (v + 1.0f) * 0.5f;

        // [0, 1] to [0, width OR height]
        const Image& image = images[faceIndex];

        const size_t width = image.getWidth();
        const size_t height = image.getHeight();
        size_t x = size_t(u * (float(width) - 1.f));
        size_t y = size_t(v * (float(height) - 1.f));
        x = std::clamp(x, size_t(0), width -1);
        y = std::clamp(y, size_t(0), height -1);
        // reverse top-bottom
        // y = height - 1 - y;

        Color pixelColor = image(x, y);

        assert(image.getAspectRatio() == 1.f);
        assert(image.getHeight() * image.getWidth() >= 1024 * 1024);

        return Vec3(pixelColor.r / 255.0f, pixelColor.g / 255.0f, pixelColor.b / 255.0f);
    }

    Vec3 calculateAmbientColor() {
        Vec3 totalColor = {0.f, 0.f, 0.f};
        int sampleCount = 1000;

        for (const auto& direction : fibonacciSphereSample(sampleCount)) {
            Vec3 sampleColor = sample(direction);
            totalColor += sampleColor;
        }

        if (sampleCount > 0) {
            totalColor /= float(sampleCount);
        }

        return totalColor;
    }

    static std::vector<Vec3> fibonacciSphereSample(int sampleCount) {
        std::vector<Vec3> points;
        points.reserve(sampleCount);

        const float phi = (1.f + std::sqrt(5.f)) / 2.f; // golden ratio
        const float ga = 2.f * PI * (1.f - 1.f / phi); // golden angle

        for (size_t i = 0; i < sampleCount; ++i) {
            float t = float(i) / (sampleCount - 1.f); // Normalize i to [0, 1]
            float inclination = std::acos(1.f - 2.f * t); // Polar angle
            float azimuth = ga * i; // Azimuthal angle

            float x = std::sin(inclination) * std::cos(azimuth);
            float y = std::sin(inclination) * std::sin(azimuth);
            float z = std::cos(inclination);

            points.emplace_back(x, y, z);
        }

        return points;
    }
};


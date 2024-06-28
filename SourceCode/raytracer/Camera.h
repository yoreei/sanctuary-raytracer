#pragma once
#include <tuple>
#include "CRTTypes.h"
#include "Image.h"
#include "SceneObject.h"

class Camera: public SceneObject
{
public:
    Camera() = default;
    Camera(float _fov, Vec3 _pos, Matrix3x3 _mat)
        : SceneObject(_pos, _mat), fov(_fov)
    {
        // TODO assert _mat is orthonormal
    }
    auto getFov() const { return fov; }
    auto getPos() const { return pos; }
    auto getDir() const { return mat.col(2); }
    auto getUp() const { return mat.col(1); }
    auto getRight() const { return mat.col(0); }

    /*
    * return ray in world space, originating from pixel (x,y) on the screen
    */
    Ray generateRay(const Image& image, int x, int y) const
    {
        Vec3 coords {static_cast<float>(x), static_cast<float>(y), 0};
        ndcFromRaster(image, coords);
        screenFromNdc(image, coords);

        // coords = coords.normalize(); skip this
        Vec3 raydir = getDir() + getRight() * coords.x + getUp() * coords.y;
        raydir = raydir.normalize();

        return Ray{this->pos, raydir};
    }

protected:

    /*
    * x, y [0, width] [0, height] -> x, y [0, 1.0] [0, 1.0]
    */
    void ndcFromRaster(const Image& image, Vec3& coordinates) const
    {
        float& x = coordinates.x;
        float& y = coordinates.y;

        // move point to center of Pixel box
        x += 0.5f;
        y += 0.5f;

        x /= image.width;
        y /= image.height;
    }

    //void screenFromNdc(Vec3& coordinates) const
    //{
    //    float& x = coordinates.x;
    //    float& y = coordinates.y;

    //    // x[0, 1] -> x[0, 2]
    //    // y[0, 1] -> y[-2, 0]
    //    x *= 2.0f;
    //    y *= -2.0f;

    //    // x[0, 2] y[0, -2] -> xy[-1, 1]
    //    x += -1.0f;
    //    y += 1.0f;

    //    x *= aspect_ratio;
    //}

    void screenFromNdc(const Image& image, Vec3& coordinates) const
    {
        float& x = coordinates.x;
        float& y = coordinates.y;

        // x[0, 1] -> x[0, 2]
        // y[0, 1] -> y[-2, 0]
        y -= 1.0f;
        x *= 2.0f;
        y *= 2.0f;

        // x[0, 2] y[0, -2] -> xy[-1, 1]
        x -= 1;
        y += 1;

        x *= image.aspectRatio;
    }

private:
    float fov = 90.0f; // Field of view in degrees
};

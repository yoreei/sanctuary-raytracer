#pragma once
#include <tuple>
#include <queue>
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
    * return unit ray in world space, originating from pixel (x,y) on the screen
    */
    void emplacePrimaryRay(const Image& image, int x, int y, std::queue<PixelRay>& queue) const
    {
        Vec3 coords {static_cast<float>(x), static_cast<float>(y), 0};
        ndcFromRaster(image, coords);
        imageFromNdc(image, coords);

        // coords = coords.normalize(); skip this
        Vec3 raydir = getDir() + getRight() * coords.x + getUp() * coords.y;
        raydir = raydir.normalize();

        queue.emplace(this->pos, raydir, x, y);
    }

    //Ray generateRay(const Image& image, int x, int y) const
    //{
    //    Vec3 coords {static_cast<float>(x), static_cast<float>(y), 0};
    //    ndcFromRaster(image, coords);
    //    imageFromNdc(image, coords);

    //    // coords = coords.normalize(); skip this
    //    Vec3 raydir = getDir() + getRight() * coords.x + getUp() * coords.y;
    //    raydir = raydir.normalize();

    //    return Ray{this->pos, raydir};
    //}

protected:

    /*
    * x, y [0, width] [0, height] -> x, y [0, 1.0] [0, 1.0]
    * example: top left raster pixel (0,0) -> (0, 0)
    * example: bottom right raster pixel (width, height) -> (1, 1)
    */
    void ndcFromRaster(const Image& image, Vec3& coordinates) const
    {
        float& x = coordinates.x;
        float& y = coordinates.y;

        // move point to center of Pixel box
        x += 0.5f;
        y += 0.5f;

        x /= image.getWidth();
        y /= image.getHeight();
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

    /*
    * returns image plane coordinates. Positive y is up, positive x is right
    * example: top left NDC (0,0) -> (-1, 1)
    * example: bottom right NDC (1,1) -> (1, -1)
    */
    void imageFromNdc(const Image& image, Vec3& coordinates) const
    {
        float& x = coordinates.x;
        float& y = coordinates.y;

        // x[0, 1] -> x[0, 2]
        // y[0, 1] -> y[-2, 0]
        x *= 2.0f;
        y *= -2.0f;

        // x[0, 2] y[0, -2] -> xy[-1, 1]
        x -= 1;
        y += 1;

        x *= image.getAspectRatio();
    }

private:
    float fov = 90.0f; // Field of view in degrees
};

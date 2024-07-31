#pragma once
#include <tuple>
#include <queue>
#include "include/CRTTypes.h"
#include "include/Image.h"
#include "include/SceneObject.h"
#include "json_fwd.h"
#include "include/Globals.h"

class Camera : public SceneObject
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
    auto getDir() const { return mat.getCol(2); }

    /* warning: does not preserve roll. Will fix this with quaternions */
    void setDir(const Vec3& dir)
    {
        assert(dir.isUnit());

        mat.setCol(2, dir);

        Vec3 right;
        Vec3{ 0.f, 1.f, 0.f }.cross(dir, right);
        if (fEqual(right.lengthSquared(), 0))
        {
            Vec3{ 1.f, 0.f, 0.f }.cross(dir, right);
        }
        right.normalize();

        Vec3 up;
        dir.cross(right, up);
        up.normalize();

        mat.setCol(0, right);
        mat.setCol(1, up);

        assert(mat.isOrthonormal());
    }

    auto getUp() const { return mat.getCol(1); }
    auto getRight() const { return mat.getCol(0); }

    void lookAt(const Vec3& dest)
    {
        if(dest.equal(pos)) {
            throw std::runtime_error("dest == pos");
        }
        Vec3 dir = dest - pos;
        dir.normalize();
        setDir(dir);
    }

    float fov = 90.0f; // Field of view in degrees

    nlohmann::ordered_json toJson() const;

    /*
    * return unit ray in world space, originating from pixel (x,y) on the screen
    */
    void emplaceTask(const Image& image, size_t x, size_t y, std::queue<TraceTask>& queue) const
    {
        Ray ray = rayFromPixel(image, x, y);
        queue.emplace(ray, x, y);
    }

    Ray rayFromPixel(const Image& image, size_t x, size_t y) const;

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

        x /= float(image.getWidth());
        y /= float(image.getHeight());
    }

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
};

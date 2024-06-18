#pragma once
#include <tuple>
#include "CRTTypes.h"

/*
* Transform from Color resolution to Normalized Device Coordinates [0, 1]
* example: x [0, 1979] -> x [0, 1]
*/

class Camera
{
public:
    Camera() = default;
    Camera(uint16_t width, uint16_t height, float fov, Vec3 pos, Vec3 dir, Vec3 up)
        : width(width), height(height), fov(fov), pos(pos), dir(dir), up(up)
    {
        aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    }
    auto getHeight() const { return height; }
    auto getWidth() const { return width; }
    auto getFov() const { return fov; }
    auto getPos() const { return pos; }
    auto getDir() const { return dir; }
    auto getUp() const { return up; }
    auto getAspectRatio() const { return aspect_ratio; }

    /*
    * return ray in world space, originating from pixel (x,y) on the screen
    */
    Ray generateRay(int x, int y) const
    {
        Vec3 raydir {static_cast<float>(x), static_cast<float>(y), dir.z};
        ndcFromRaster(raydir);
        screenFromNdc(raydir);

        raydir = raydir.normalize();

        return Ray{this->pos, raydir};
    }

    /*
    * Move forwards/ backwards
    */
    void dolly(float dist)
    {
        pos = pos + dir * dist;
    }

    /*
    * Raise upwards or sink downwards
    */ 
    void pedestal(float dist)
    {
        pos = pos + up * dist;
    }

    /*
    * Stafe left or right
    */
    void truck(float dist)
    {

    }

    /*
    * Rotate around Y axis (as if camera direction = (0,0,-1))
    */
    void pan(float deg)
    {

    }

    /*
    * Rotate up/down around X axis (as if camera direction = (0,0,-1))
    */
    void tilt(float deg)
    {

    }

    /*
    * Rotate around Z axis (as if camera direction = (0,0,-1))
    */
    void roll(float deg)
    {
    }

protected:

    /*
    * x, y [0, width] [0, height] -> x, y [0, 1.0] [0, 1.0]
    */
    void ndcFromRaster(Vec3& coordinates) const
    {
        float& x = coordinates.x;
        float& y = coordinates.y;

        // move point to center of Pixel box
        x += 0.5f;
        y += 0.5f;

        x /= width;
        y /= height;
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

    void screenFromNdc(Vec3& coordinates) const
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

        x *= aspect_ratio;
    }

private:
    /* Resolution of the camera */
    uint16_t width = 800;
    uint16_t height = 600;
    // Assuming horizontal screen orientation
    float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    float fov = 90.0f; // Field of view in degrees

    /* Position. World Space */
    Vec3 pos {0.f, 0.f, 0.f};

    /* Direction to look at. World Space */
    Vec3 dir {0.f, 0.f, -1.f};

    /* Up vector used for camera rotations. World Space */
    Vec3 up {0.f, 1.f, 0.f};
};

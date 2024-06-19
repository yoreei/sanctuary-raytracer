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
    Camera(uint16_t _width, uint16_t _height, float _fov, Vec3 _pos, Matrix3x3 _mat)
        : width(_width), height(_height), fov(_fov), pos(_pos), mat(_mat)
    {
        aspect_ratio = static_cast<float>(_width) / static_cast<float>(_height);
    }
    auto getHeight() const { return height; }
    auto getWidth() const { return width; }
    auto getFov() const { return fov; }
    auto getPos() const { return pos; }
    auto getDir() const { return mat.col(2); }
    auto getUp() const { return mat.col(1); }
    auto getRight() const {return mat.col(0); }
    auto getAspectRatio() const { return aspect_ratio; }

    /*
    * return ray in world space, originating from pixel (x,y) on the screen
    */
    Ray generateRay(int x, int y) const
    {
        Vec3 coords {static_cast<float>(x), static_cast<float>(y), 0};
        ndcFromRaster(coords);
        screenFromNdc(coords);

        // coords = coords.normalize(); skip this
        Vec3 raydir = getDir() + getRight() * coords.x + getUp() * coords.y;
        raydir = raydir.normalize();

        return Ray{this->pos, raydir};
    }

    /*
    * Move forwards/ backwards
    */
    void dolly(float dist)
    {
        // 3rd col is forward direction
        Vec3 dir = mat.col(2);
        pos = pos + dir * dist;
    }

    /*
    * Raise upwards or sink downwards
    */ 
    void pedestal(float dist)
    {
        // 2nd col is up direction
        Vec3 up = mat.col(1);
        pos = pos + up * dist;
    }

    /*
    * Stafe left or right
    */
    void truck(float dist)
    {
        // 1st col is right direction
        Vec3 right = mat.col(0);
        pos = pos + right * dist;
    }

    /*
    * Rotate around camera's Y axis
    */
    void pan(float deg)
    {
        mat = Matrix3x3::pan(deg) * mat;
    }

    /*
    * Rotate up/down around camera's X axis
    */
    void tilt(float deg)
    {
        mat = Matrix3x3::tilt(deg) * mat;
    }

    /*
    * Rotate around camera's Z axis
    */
    void roll(float deg)
    {
        mat = Matrix3x3::roll(deg) * mat;
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

    /*
    * Stores camera orientation. 3rd col is forward direction, 2nd col is up direction, 1st col is right direction.
    */
    Matrix3x3 mat = Matrix3x3 {{
    1, 0, 0,
    0, 1, 0,
    0, 0, -1
    }};
};

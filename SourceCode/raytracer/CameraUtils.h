#pragma once
#include "CRTTypes.h"

void NDCFromRaster(Vec3& point, int width, int height)
{
    // move point to center of pixel
    point = point + Vec3{0.5, 0.5, 0.0};
    point.x /= width;
    point.y /= height;
    
    /* Alternative implementation:
    Matrix3x3 NDCFromRasterMat = {
        {1.0f / width, 0.0f,          0.0f,
        0.0f,          1.0f / height, 0.0f,
        0.0f,          0.0f,          1.0f}
    };
    */
}

void ScreenFromNDC(Vec3& point, float aspect_ratio)
{
    // x[0, 1] -> x[0, 2]
    // y[0, 1] -> y[0, -2]
    point.x *= 2.0f;
    point.y *= -2.0f;

    // x[0, 2] y[0, -2] -> xy[-1, 1]
    point = point + Vec3{-1.0, 1.0, 0.0};
    point.x *= aspect_ratio;

    /* Alternative implementation:
    Matrix3x3 ScreenFromNDCMat = {
        {2.0f, 0.0f, 0.0f,
        0.0f, -2.0f, 0.0f,
        0.0f, 0.0f, 1.0f}
    };

    Matrix3x3 AspectRatioMat = {
        {aspect_ratio, 0.0f, 0.0f,
        0.0f,            1.0f, 0.0f,
        0.0f,            0.0f, 1.0f}
    };
    */
}

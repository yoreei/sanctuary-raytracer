#pragma once
#include "CRTTypes.h"

Vec3 NDCFromRaster(const Vec3& point, int width, int height)
{
    // move point to center of pixel
    Vec3 result = point + Vec3{0.5, 0.5, 0.0};

    Matrix3x3 NDCFromRasterMat = {
        {1.0f / width, 0.0f,          0.0f,
        0.0f,          1.0f / height, 0.0f,
        0.0f,          0.0f,          1.0f}
    };
    return NDCFromRasterMat * result;
}

Vec3 ScreenFromNDC(const Vec3& point, float aspect_ratio)
{
    Vec3 result = point;

    // x[0, 1] -> x[0, 2]
    // y[0, 1] -> y[0, -2]
    Matrix3x3 ScreenFromNDCMat = {
        {2.0f, 0.0f, 0.0f,
        0.0f, -2.0f, 0.0f,
        0.0f, 0.0f, 1.0f}
    };
    result = ScreenFromNDCMat * result;

    // x[0, 2] y[0, -2] -> xy[-1, 1]
    result = result + Vec3{-1.0, 1.0, 0.0};

    Matrix3x3 AspectRatioMat = {
        {aspect_ratio, 0.0f, 0.0f,
        0.0f,            1.0f, 0.0f,
        0.0f,            0.0f, 1.0f}
    };
    return AspectRatioMat * result;
}
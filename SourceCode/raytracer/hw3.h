#pragma once
#include "CRTTypes.h"
#include "CameraUtils.h"

void hw3(Buffer& buf)
{
    for (int y = 0; y < buf.height; ++y) {
        for (int x = 0; x < buf.width; ++x) {
            Vec3 ray = {(float)x, (float)y, -1.0};
            ray = NDCFromRaster(ray, buf.width, buf.height);
            ray = ScreenFromNDC(ray, buf.width / buf.height);
            ray = ray.normalize();
            Vec3 color = ray * 255;

            Pixel& pixel = buf.data[y * buf.width + x];
            pixel.r = (uint8_t)(color.x);
            pixel.g = (uint8_t)(color.y);
            pixel.b = (uint8_t)(color.z);
        }
    }
}

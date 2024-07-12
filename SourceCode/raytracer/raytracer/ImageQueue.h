#pragma once
#include <vector>
#include <queue>
#include "CRTTypes.h"

class ColorContrib {
public:
    ColorContrib(Vec3 color, float attenuation);
    Vec3 color;
    float attenuation;
};

class Image;
using PixelQueue = std::queue<ColorContrib>;
class ImageQueue
{
public:
    // TODO: refactor width, height to pixelHandle
    ImageQueue(size_t width, size_t height, const Vec3& bgColor);
    void flatten(Image& image);
    void slice(std::vector<Image>& images);
    size_t maxDepth() const;
    PixelQueue& operator()(size_t x, size_t y);
    std::vector<PixelQueue> data;
    size_t width;
    size_t height;
private:
    Vec3 bgColor;
    // A single ray's contribution to the final pixel color
};


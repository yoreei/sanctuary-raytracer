#pragma once
#include <vector>
#include <queue>
#include "CRTTypes.h"

/* A single ray's contribution to the final pixel color */
class ColorContrib {
public:
    ColorContrib(Vec3 color, float weight);
    Vec3 color;
    float weight;
};

class Image;
using PixelQueue = std::queue<ColorContrib>;

/*
* Records each ray's contribution to a pixel. Needs to be flattened to an Image before output.
*/
class ImageQueue
{
public:
    // TODO: refactor width, height to pixelHandle
    ImageQueue(size_t width, size_t height, const Vec3& bgColor);

    /* @ouput image: flattened image */
    void flatten(Image& image);

    /* Output each depth as its own image. Useful for debugging */
    void slice(std::vector<Image>& images);
    size_t maxDepth() const;
    PixelQueue& operator()(size_t x, size_t y);
    std::vector<PixelQueue> data;
    size_t width;
    size_t height;
private:
    Vec3 bgColor;
};


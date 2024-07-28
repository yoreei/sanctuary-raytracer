#pragma once
#include <vector>
#include <queue>
#include "CRTTypes.h"

enum class BlendType {
    NORMAL, /* Source Over */
    ADDITIVE,
};
/* A single ray's contribution to the final pixel color */
class ColorContrib {
public:
    Vec3 color = { 0.f, 0.f, 0.f };
    float weight = 1.f;
    BlendType blendType = BlendType::NORMAL;
};

class Image;
using PixelQueue = std::queue<ColorContrib>;

/*
* Records each ray's contribution to a pixel. Needs to be flattened to an Image before output.
*/
class ShadingSamples
{
public:
    // TODO: refactor width, height to pixelHandle
    ShadingSamples(size_t width, size_t height);

    /* @ouput image: flattened image */
    void flatten(Image& image);

    /* Output each depth as its own image. Useful for debugging */
    void slice(std::vector<Image>& images);
    size_t maxDepth() const;
    PixelQueue& operator()(size_t x, size_t y);
    std::vector<PixelQueue> data;
    size_t width;
    size_t height;
};


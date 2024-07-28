#include "ShadingSamples.h"

#include <algorithm>
#include "Image.h"
#include "CRTTypes.h"

ShadingSamples::ShadingSamples(size_t width, size_t height): width(width), height(height)
{
    data.resize(width * height, {});
}

PixelQueue& ShadingSamples::operator()(size_t x, size_t y)
{
    if (x >= width || y >= height)
    {
        throw std::out_of_range("");
    }
    return data[y * width + x];
}

void ShadingSamples::flatten(Image& image)
{
    assert(image.getWidth() == width && image.getHeight() == height);

    // for each pixel
    for (size_t i = 0; i < width * height; ++i) {
        Vec3 unitColor = {0.f, 1.f, 0.f}; // should not appear in final image. TODO remove

        // and each shade color
        while (!data[i].empty()) {
            ColorContrib& layer= data[i].front();

            if (layer.blendType == BlendType::NORMAL) {
                unitColor = lerp(unitColor, layer.color, layer.weight);
            }

            else if (layer.blendType == BlendType::ADDITIVE) {
                unitColor = (unitColor + layer.color);
                unitColor.clamp(0.f, 1.f);
            }
            data[i].pop();

        }
        image.data[i] = Color::fromUnit(unitColor);
    }
}

void ShadingSamples::slice(std::vector<Image>& images) {
    assert(images.size() == 0);
    size_t depth = 0;
    bool bContinue = true;
    while(bContinue){
        images.emplace_back(width, height);
        bContinue = false;
        for (size_t i = 0; i < width * height; ++i) {
            if(!data[i].empty()) {
                bContinue = true;
                ColorContrib& layer = data[i].front();
                Vec3 unitColor = lerp({0.f, 0.f, 0.f}, layer.color, layer.weight);
                images[depth].data[i] = Color::fromUnit(unitColor);
                data[i].pop();
            }
            else {
                images[depth].data[i] = Color{0, 0, 0};
            }
        }
        ++depth;
    }
}

size_t ShadingSamples::maxDepth() const
{
    size_t max = 0;
    for (const PixelQueue& queue : data) {
        max = std::max(queue.size(), max);
    }
    return max;
}


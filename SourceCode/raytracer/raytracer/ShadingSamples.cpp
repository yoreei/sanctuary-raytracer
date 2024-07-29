#include "ShadingSamples.h"

#include <algorithm>
#include "Image.h"
#include "CRTTypes.h"
#include "Settings.h"

ShadingSamples::ShadingSamples(size_t width, size_t height, const Settings* settings): width(width), height(height), settings(settings)
{
    pixels = std::vector<Shades>(width * height);
}

const Shades& ShadingSamples::operator()(size_t x, size_t y)
{
    if (x >= width || y >= height)
    {
        throw std::out_of_range("");
    }
    return pixels[y * width + x];
}

void ShadingSamples::addSample(const TraceTask& task, const Vec3 color, BlendType blendType)
{ 
    if (task.pixelX >= width || task.pixelY >= height)
    {
        throw std::out_of_range("");
    }
    Shades& shades = pixels[task.pixelY * width + task.pixelX];
    
    if (shades.size() == 0 && task.weight != 1.f) {
        throw std::runtime_error("first weight should be 1.f");
    }

    shades.emplace_back(color, task.weight, blendType);
}


void ShadingSamples::flatten(Image& image)
{
    assert(image.getWidth() == width && image.getHeight() == height);

    // for each pixel index
    for (size_t pIdx = 0; pIdx < width * height; ++pIdx) {
        Shades& shades = pixels[pIdx];

        if (shades.size() == 0) {
            if (settings->debugPixel) {
                continue;
            }
            else {
                throw std::runtime_error("pixel has no data");
            }
        }

        // ... and each of its shades
        Vec3 result;
        result = shades[0].color;
        assert(shades[0].weight == 1.f);
        for (size_t sIdx = 1; sIdx < shades.size(); ++sIdx) {
            Shade& shade = shades[sIdx];

            if (shade.blendType == BlendType::NORMAL) {
                result = lerp(result, shade.color, shade.weight);
            }

            else if (shade.blendType == BlendType::ADDITIVE) {
                result = (result + shade.color);
                result.clamp(0.f, 1.f);
            }
            else {
                throw std::runtime_error("unknown BlendType");
            }

        }

        shades.clear();
        image.data[pIdx] = Color::fromUnit(result);
    }
}

void ShadingSamples::slice(std::vector<Image>& images) {
    assert(images.size() == 0);
    
    // Prepare images vector
    images.clear();
    images.reserve(pixelsMaxDepth());
    for (size_t i = 0; i < pixelsMaxDepth(); ++i) {
        images.emplace_back(width, height);
    }

    // From array of stacks (pixels) To stacks of arrays (images)
    for (size_t pIdx = 0; pIdx < width * height; ++pIdx) {
        const Shades& shades = pixels[pIdx];

        for(size_t sIdx = 0; sIdx < shades.size(); ++sIdx) {
            const Shade& shade = shades[sIdx];
            images[sIdx].data[pIdx] = Color::fromUnit(shade.color);
        }
    }
}

size_t ShadingSamples::pixelsMaxDepth() {
    size_t maxDepth = 0;
    for(const auto& pixel : pixels) {
        maxDepth = std::max(pixel.size(), maxDepth);
    }
    return maxDepth;
}

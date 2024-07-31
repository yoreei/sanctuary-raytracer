#include "include/ShadingSamples.h"

#include <algorithm>
#include "include/Image.h"
#include "include/CRTTypes.h"
#include "include/Settings.h"

ShadingSamples::ShadingSamples(size_t width, size_t height, const Settings* settings): width(width), height(height), settings(settings)
{
    pixels = std::vector<Shades>(width * height);
}

// todo remove?
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
    
    //if (shades.size() == 0 && task.weight != 1.f) {
    //    throw std::runtime_error("first weight should be 1.f");
    //}

    shades.emplace_back(color, task.weight, blendType);
}

Vec3 flattenShades(const Shades& shades)
{
    Vec3 result;
    result = shades[0].color;
    // assert(shades[0].weight == 1.f); todo remove
    for (size_t sIdx = 1; sIdx < shades.size(); ++sIdx) {
        const Shade& shade = shades[sIdx];

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
    return result;
}

void ShadingSamples::flatten(Image& image)
{
    assert(image.getWidth() == width && image.getHeight() == height);

    // for each pixel index
    for (size_t y = image.startPixelY; y < image.endPixelY; ++y) {
        for (size_t x = image.startPixelX; x < image.endPixelX; ++x) {
            Shades& shades = pixels[y * width + x];
            if (shades.size() == 0) {
                std::string pixelStr = std::to_string(y) + ", " + std::to_string(x);
                throw std::runtime_error("pixel " + pixelStr + " has no data");
            }

            Vec3 result = flattenShades(shades);
            shades.clear();
            image(x,y) = Color::fromUnit(result);
        }
    }
}

void ShadingSamples::slice(std::vector<Image>& images) {
    assert(images.size() == 0);
    
    // Prepare images vector
    images.clear();
    size_t maxDepth = pixelsMaxDepth();
    images.reserve(maxDepth);
    for (size_t i = 0; i < maxDepth; ++i) {
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

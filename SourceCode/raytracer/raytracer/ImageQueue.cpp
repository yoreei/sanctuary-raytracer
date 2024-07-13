#include "ImageQueue.h"
#include "Image.h"
#include "CRTTypes.h"

ImageQueue::ImageQueue(size_t width, size_t height, const Vec3& bgColor): width(width), height(height), bgColor(bgColor)
{
    data.resize(width * height, {});
}

PixelQueue& ImageQueue::operator()(size_t x, size_t y)
{
    if (x >= width || y >= height)
    {
        throw std::out_of_range("");
    }
    return data[y * width + x];
}

void ImageQueue::flatten(Image& image)
{
    assert(image.getWidth() == width && image.getHeight() == height);
    for (int i = 0; i < width * height; i++) {
        Vec3 unitColor = bgColor;
        while (!data[i].empty()) {
            ColorContrib& layer= data[i].front();
            unitColor = lerp(unitColor, layer.color, layer.weight);
            data[i].pop();
        }
        image.data[i] = Color::fromUnit(unitColor);
    }
}

void ImageQueue::slice(std::vector<Image>& images) {
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

size_t ImageQueue::maxDepth() const
{
    size_t max = 0;
    for (const PixelQueue& queue : data) {
        max = std::max(queue.size(), max);
    }
    return max;
}

ColorContrib::ColorContrib(Vec3 color, float weight) : color(color), weight(weight) {}

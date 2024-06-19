#pragma once

#include<memory>
#include "CRTTypes.h"

struct Image {
    Image(int _width, int _height): width(_width), height(_height)
    {
        data = std::make_unique<Color[]>(width * height);
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }

    Color& operator()(uint16_t x, uint16_t y)
    {
        if (x >= width || y >= height)
        {
            throw std::out_of_range("Buffer2D::operator(): Index out of range");
        }
        return data[y * width + x];
    }

    std::unique_ptr<Color[]> data;

    uint16_t width = 0;
    uint16_t height = 0;
    float aspectRatio = 0.f;

    std::string toPpm(const Image& buf)
    {
        std::ofstream ppmFileStream("crt_output_image.ppm", std::ios::out | std::ios::binary);
        ppmFileStream << "P3\n"; ppmFileStream << buf.width << " " << buf.height << "\n";
        ppmFileStream << Color::maxColorComponent << "\n";

        for (int rowIdx = 0; rowIdx < buf.height; ++rowIdx) {
            for (int colIdx = 0; colIdx < buf.width; ++colIdx) {
                const Color& Color = buf.data[rowIdx * buf.width + colIdx];
                ppmFileStream << (int)Color.r << " " << (int)Color.g << " " << (int)Color.b << "\t";
            }
            ppmFileStream << "\n";
        }

        ppmFileStream.close();
    }

    std::string toPpmString() {
        std::string result;
        result.reserve(20 + (width * height * 12));

        result += "P3\n";
        result += std::to_string(width) + " " + std::to_string(height) + "\n";
        result += std::to_string(Color::maxColorComponent) + "\n";

        for (int rowIdx = 0; rowIdx < height; ++rowIdx) {
            for (int colIdx = 0; colIdx < width; ++colIdx) {
                const Color& color = data[rowIdx * width + colIdx];
                result += std::to_string(color.r) + " " + std::to_string(color.g) + " " + std::to_string(color.b) + "\t";
            }
            result += "\n";
        }

        return result;
    }
};
#pragma once

#include<memory>

#include "CRTTypes.h"

#include "stb_image_write.h"

class Image {
public:
    Image() = default;

    Image(size_t _width, size_t _height): width(_width), height(_height)
    {
        data = std::make_unique<Color[]>(width * height);
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }

    auto getWidth() const { return width; }
    auto getHeight() const { return height; }
    auto getAspectRatio() const { return aspectRatio; }

    Color& operator()(size_t x, size_t y)
    {
        if (x >= width || y >= height)
        {
            throw std::out_of_range("Image::operator(): Index out of range");
        }
        return data[y * width + x];
    }

    std::unique_ptr<Color[]> data;

    void writeToPpm(const std::string& filename)
    {
        std::ofstream ppmFileStream(filename, std::ios::out | std::ios::binary);
        if (!ppmFileStream.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        ppmFileStream << "P3\n"; ppmFileStream << width << " " << height << "\n";
        ppmFileStream << Color::maxColorComponent << "\n";

        for (size_t rowIdx = 0; rowIdx < height; ++rowIdx) {
            for (int colIdx = 0; colIdx < width; ++colIdx) {
                const Color& color = data[rowIdx * width + colIdx];
                ppmFileStream << (int)color.r << " " << (int)color.g << " " << (int)color.b << "\t";
            }
            ppmFileStream << "\n";
        }

        ppmFileStream.close();
        if (ppmFileStream.fail()) {
            std::cerr << "Failed to write to file: " << filename << std::endl;
        }
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

    void writeToPng(const std::string& filename) const {
        std::vector<uint8_t> pngData(width * height * 3);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const Color& color = data[y * width + x];
                pngData[3 * (y * width + x) + 0] = color.r;
                pngData[3 * (y * width + x) + 1] = color.g;
                pngData[3 * (y * width + x) + 2] = color.b;
            }
        }
        int stride_bytes = static_cast<int>(width) * 3;
        stbi_write_png(filename.c_str(), static_cast<int>(width), static_cast<int>(height), 3, pngData.data(), stride_bytes);
    }

private:
    size_t width = 1;
    size_t height = 1;
    float aspectRatio = 1.f;
};

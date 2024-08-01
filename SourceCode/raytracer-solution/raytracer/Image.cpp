#include "include/Image.h"
#include <fstream>

inline void Image::writeToPpm(const std::string& filename)
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

std::string Image::toPpmString() {
    std::string result;
    result.reserve(20 + (width * height * 12));

    result += "P3\n";
    result += std::to_string(width) + " " + std::to_string(height) + "\n";
    result += std::to_string(Color::maxColorComponent) + "\n";

    for (size_t rowIdx = 0; rowIdx < height; ++rowIdx) {
        for (size_t colIdx = 0; colIdx < width; ++colIdx) {
            const Color& color = data[rowIdx * width + colIdx];
            result += std::to_string(color.r) + " " + std::to_string(color.g) + " " + std::to_string(color.b) + "\t";
        }
        result += "\n";
    }

    return result;
}

void Image::writeImage(std::string filename, bool writePng, bool writeBmp) const {
    std::vector<uint8_t> pngData(width * height * 3);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            const Color& color = data[y * width + x];
            pngData[3 * (y * width + x) + 0] = color.r;
            pngData[3 * (y * width + x) + 1] = color.g;
            pngData[3 * (y * width + x) + 2] = color.b;
        }
    }
    int channels = 3;
    int intWidth = static_cast<int>(width);
    int intHeight = static_cast<int>(height);

    if (writePng) {
        int stride_bytes = static_cast<int>(width) * 3;
        std::string filenameWithExt = filename + ".png";
        int result = stbi_write_png(filenameWithExt.c_str(), intWidth, intHeight, channels, pngData.data(), stride_bytes);
        if (result == 0) {
            throw std::runtime_error("Failed to write PNG file");
        }
    }
    if (writeBmp) {
        std::string filenameWithExt = filename + ".bmp";
        int result = stbi_write_bmp(filenameWithExt.c_str(), intWidth, intHeight, channels, pngData.data());
        if (result == 0) {
            throw std::runtime_error("Failed to write BMP file");
        }
    }

}

#include "Image.h"
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

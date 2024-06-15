#include <fstream>
#include <cstdint>
#include <memory>
#include <string>

#include "CRTTypes.h"
#include "hw3.h"
#include "hw4.h"

/// Output image resolution
static const int imageWidth = 1920;
static const int imageHeight = 1080;
static const int maxColorComponent = 255;

void save_ppm(Buffer2D& buf)
{
    std::ofstream ppmFileStream("crt_output_image.ppm", std::ios::out | std::ios::binary);
    ppmFileStream << "P3\n"; ppmFileStream << buf.width << " " << buf.height << "\n";
    ppmFileStream << maxColorComponent << "\n";

    for (int rowIdx = 0; rowIdx < buf.height; ++rowIdx) {
        for (int colIdx = 0; colIdx < buf.width; ++colIdx) {
            const Pixel& pixel = buf.data[rowIdx * buf.width + colIdx];
            ppmFileStream << (int)pixel.r << " " << (int)pixel.g << " " << (int)pixel.b << "\t";
        }
        ppmFileStream << "\n";
    }

    ppmFileStream.close();
}

int main()
{
    // hw4
    hw4();

    // hw3
    Buffer2D buf = {imageWidth, imageHeight};
    hw3(buf);
    save_ppm(buf);
    return 0;
}

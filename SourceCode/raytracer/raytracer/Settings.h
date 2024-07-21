#pragma once

class Settings {
public:
    std::string inputDir = "scenes/in";
    size_t maxDepth = 16;
    float bias = 0.001f;
    bool forceSingleThread = false;

    bool debugPixel = true;
    size_t debugPixelX = 0;
    size_t debugPixelY = 0;

    bool debugLight = false;
    bool debugImageQueue = false;
};
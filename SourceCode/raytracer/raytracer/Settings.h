#pragma once

class Settings {
public:
    std::string inputDir = "scenes/in";
    size_t maxDepth = 16;
    float bias = 0.001f;
    bool debugSingleRay = false;
    bool debugLight = false;
    bool debugImageQueue = true;
};
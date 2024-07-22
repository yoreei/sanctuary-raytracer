#pragma once
#include <string>

class Settings {
public:
    std::string sceneLibraryDir = "scene_library";
    std::string projectDir = "hw13";

    size_t maxDepth = 16;
    float bias = 0.001f;
    bool forceSingleThread = false;

    bool debugPixel = true;
    size_t debugPixelX = 0;
    size_t debugPixelY = 0;

    bool debugLight = false;
    bool debugImageQueue = false;

    bool overrideResolution = false;
    size_t resolutionX = 300;
    size_t resolutionY = 200;

    static Settings load(const std::string& filename);
};
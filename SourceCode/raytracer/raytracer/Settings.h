#pragma once
#include <string>

class Settings {
public:
    // File paths
    std::string sceneLibraryDir = "scene_library";
    std::string projectDir = "hw13";
    std::vector<std::string> targetScenes {"scene0.crtscene"};

    // Rendering settings
    size_t maxDepth = 16;
    float bias = 0.001f;
    bool forceSingleThread = false;

    // Debug settings
    bool debugPixel = true;
    size_t debugPixelX = 0;
    size_t debugPixelY = 0;

    bool debugLight = false;
    bool debugImageQueue = false;

    // Resolution settings
    bool overrideResolution = false;
    size_t resolutionX = 300;
    size_t resolutionY = 200;

    // Acceleration structure settings
    bool debugAccelStructure = false;

    // Functions
    bool loadEntireProject() const;
    static Settings load(const std::string& filename);
};
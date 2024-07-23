#pragma once
#include <string>

class Settings {
public:
    // Unique ID for each render iteration
    size_t settingsId = 0;

    // File paths
    std::string sceneLibraryDir = "sceneLibrary";
    std::string projectDir = "hw13";
    std::vector<std::string> targetScenes {"scene0.crtscene"};
    std::string outputDir = "out";

    // Rendering settings
    size_t maxDepth = 16;
    float bias = 0.001f;

    // Debug settings
    bool debugPixel = true;
    size_t debugPixelX = 0;
    size_t debugPixelY = 0;

    bool debugLight = false;
    bool debugImageQueue = false;
    bool debugAccelStructure = false;

    // Resolution settings
    bool overrideResolution = false;
    size_t resolutionX = 300;
    size_t resolutionY = 200;

    // Output settings. TODO: add to parse
    bool bWritePng = true;
    bool bWriteBmp = true;

    // Optimization Settings
    bool forceNoAccelStructure = false;
    bool forceSingleThreaded = false;

    // Functions
    std::string iterationName() const;
    std::string iterationPathNoExt() const;
    std::string framePathNoExt(const std::string& sceneName, size_t frameNumber) const;
    bool loadEntireProject() const;
    static std::vector<Settings> load(const std::string& filename);
    std::string toString() const;
};
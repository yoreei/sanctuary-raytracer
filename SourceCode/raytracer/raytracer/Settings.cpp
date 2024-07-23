#include <string>
#include <fstream>

#include "json.hpp"

#include "Settings.h"

using json = nlohmann::json;

bool Settings::loadEntireProject() const
{
    return targetScenes.empty();
}

Settings Settings::load(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to load CRTScene file: " + filename);
    }

    Settings settings;
    json j;
    file >> j;

    settings.sceneLibraryDir = j.at("sceneLibraryDir");
    settings.projectDir = j.at("projectDir");
    settings.targetScenes = j.at("targetScenes");

    settings.maxDepth = j.at("maxDepth");
    settings.bias = j.at("bias");
    settings.forceSingleThread = j.at("forceSingleThread");

    settings.debugPixel = j.at("debugPixel");
    settings.debugPixelX = j.at("debugPixelX");
    settings.debugPixelY = j.at("debugPixelY");

    settings.debugLight = j.at("debugLight");
    settings.debugImageQueue = j.at("debugImageQueue");

    settings.overrideResolution = j.at("overrideResolution");
    settings.resolutionX = j.at("resolutionX");
    settings.resolutionY = j.at("resolutionY");

    settings.debugAccelStructure = j.at("debugAccelStructure");
    return settings;
}


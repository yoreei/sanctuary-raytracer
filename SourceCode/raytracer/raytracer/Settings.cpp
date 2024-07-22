#include <string>
#include <fstream>

#include "json.hpp"

#include "Settings.h"

using json = nlohmann::json;

Settings Settings::load(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to load CRTScene file: " + filename);
    }

    json j;
    file >> j;

    Settings settings;
    settings.sceneLibraryDir = j.at("sceneLibraryDir").get<std::string>();
    settings.projectDir = j.at("projectDir").get<std::string>();

    settings.maxDepth = j.at("maxDepth").get<size_t>();
    settings.bias = j.at("bias").get<float>();
    settings.forceSingleThread = j.at("forceSingleThread").get<bool>();

    settings.debugPixel = j.at("debugPixel").get<bool>();
    settings.debugPixelX = j.at("debugPixelX").get<size_t>();
    settings.debugPixelY = j.at("debugPixelY").get<size_t>();

    settings.debugLight = j.at("debugLight").get<bool>();
    settings.debugImageQueue = j.at("debugImageQueue").get<bool>();

    settings.overrideResolution = j.at("overrideResolution").get<bool>();
    settings.resolutionX = j.at("resolutionX").get<size_t>();
    settings.resolutionY = j.at("resolutionY").get<size_t>();
    
    return settings;
}

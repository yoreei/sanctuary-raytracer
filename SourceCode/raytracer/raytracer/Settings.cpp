#include <string>
#include <fstream>

#include "json.hpp"

#include "Settings.h"

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

std::string Settings::iterationName() const
{
    return "Settings" + std::to_string(settingsId);
}

bool Settings::loadEntireProject() const
{
    return targetScenes.empty();
}

std::vector<Settings> Settings::load(const std::string& filename)
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
    return { settings };
}

std::string Settings::toString() const {
    ordered_json j;
    j["settingsId"] = settingsId;
    j["sceneLibraryDir"] = sceneLibraryDir;
    j["projectDir"] = projectDir;
    j["targetScenes"] = targetScenes;
    j["maxDepth"] = maxDepth;
    j["bias"] = bias;
    j["debugPixel"] = debugPixel;
    j["debugPixelX"] = debugPixelX;
    j["debugPixelY"] = debugPixelY;
    j["debugLight"] = debugLight;
    j["debugImageQueue"] = debugImageQueue;
    j["debugAccelStructure"] = debugAccelStructure;
    j["overrideResolution"] = overrideResolution;
    j["resolutionX"] = resolutionX;
    j["resolutionY"] = resolutionY;
    j["forceSingleThread"] = forceSingleThread;
    j["forceNoAccelStructure"] = forceNoAccelStructure;

    return j.dump(4);
}

std::string Settings::iterationPathNoExt() const {
    return outputDir + "/" + iterationName();
}
std::string Settings::framePathNoExt(const std::string& sceneName, size_t frameNumber) const {
    return outputDir + "/" + iterationName() + "/" + projectDir + "/" + sceneName + "/" + std::to_string(frameNumber);
}

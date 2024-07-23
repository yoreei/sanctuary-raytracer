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

    settings.sceneLibraryDir = j.value("sceneLibraryDir", "scene_library");
    settings.projectDir = j.value("projectDir", "hw13");
    settings.targetScenes = j.value("targetScenes", std::vector<std::string>{"scene0.crtscene"});
    settings.outputDir = j.value("outputDir", "out");

    settings.maxDepth = j.value("maxDepth", 16);
    settings.bias = j.value("bias", 0.001f);

    settings.debugPixel = j.value("debugPixel", true);
    settings.debugPixelX = j.value("debugPixelX", 0);
    settings.debugPixelY = j.value("debugPixelY", 0);

    settings.debugLight = j.value("debugLight", false);
    settings.debugImageQueue = j.value("debugImageQueue", false);
    settings.debugAccelStructure = j.value("debugAccelStructure", false);

    settings.overrideResolution = j.value("overrideResolution", false);
    settings.resolutionX = j.value("resolutionX", 300);
    settings.resolutionY = j.value("resolutionY", 200);

    settings.bWritePng = j.value("bWritePng", true);
    settings.bWriteBmp = j.value("bWriteBmp", true);

    settings.forceNoAccelStructure = j.value("forceNoAccelStructure", false);
    settings.forceSingleThreaded = j.value("forceSingleThreaded", false);

    return { settings };
}

std::string Settings::toString() const {
    ordered_json j;

    j["sceneLibraryDir"] = sceneLibraryDir;
    j["projectDir"] = projectDir;
    j["targetScenes"] = targetScenes;
    j["outputDir"] = outputDir;

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

    j["bWritePng"] = bWritePng;
    j["bWriteBmp"] = bWriteBmp;

    j["forceNoAccelStructure"] = forceNoAccelStructure;
    j["forceSingleThreaded"] = forceSingleThreaded;

    return j.dump(4);  // 4 spaces for pretty print
}

std::string Settings::iterationPathNoExt() const {
    return outputDir + "/" + iterationName();
}
std::string Settings::framePathNoExt(const std::string& sceneName, size_t frameNumber) const {
    return outputDir + "/" + iterationName() + "/" + projectDir + "/" + sceneName + "/" + std::to_string(frameNumber);
}

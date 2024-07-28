/*
* filename: Settings.h
*
* GPT Prompt: Generate the corresponding implementation file (Settings.cpp) with all functions implemented as per this header file. The output should consist only of valid C++ code, including necessary comments, and should not contain any explanations or additional text. Ensure all code is syntactically correct and complete.
*
* Comment to Human: Copy and paste this header into a GPT to receive implementation
*/

#pragma once
#include <string>
#include <vector>

class Settings {
public:
    // Unique ID for each render iteration. Not read from file
    /* GPT Instruction: Do NOT parse settingsId from file */
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
    bool enableShadingSamples = false;
    bool debugAccelStructure = false;

    bool showAabbs = false;

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
    size_t maxTrianglesPerLeaf = 4;
    size_t accelTreeMaxDepth = 12345;

    // GPT Generated Functions
    /* GPT Instruction: parse each element using json.at(elem). Do not check if `elem` exists */
    /* @brief: Will not catch exceptions from nlohmann::json if any key is missing */
    static Settings load(const std::string& filename = "settings.json");
    
    /* Format back to Json-formatted string */
    std::string toString() const;

    // Human-written functions.
    // GPT Instruction: ignore these
    std::string projectPath() const;
    std::string iterationName() const;
    std::string iterationPathNoExt() const;
    std::string framePathNoExt(const std::string& sceneName, size_t frameNumber) const;
    bool loadEntireProject() const;
};
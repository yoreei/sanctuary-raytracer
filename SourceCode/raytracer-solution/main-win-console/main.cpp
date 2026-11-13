/* Main entry for Windows console. Reads settings and starts the engine */
#include <iostream>
#include <fstream>
#include <filesystem>

#include "include/Engine.h"
#include "include/Settings.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

/* @brief Output benchmarking information for a settings permutation */
void logSettingsIteration(const Settings& settings) {
    std::ofstream logFile;
    logFile.open("out/" + settings.iterationName() + ".json", std::ios::out);
    logFile << settings.toString() << std::endl;
    std::cout << settings.toString() << std::endl;
    logFile.close();
}

/* @brief Add extra parameters */
void addBenchmarkingPermutations(std::vector<Settings>& settingsList)
{
    settingsList.back().accelTreeMaxDepth = 2;
    settingsList.back().maxTrianglesPerLeaf = 2;

    for (int i = 1; i < 30; ++i) {
        for (int j = 0; j < 30; ++j)
        {
            settingsList.push_back(settingsList.back());
            settingsList.back().settingsId += 1;
            settingsList.back().maxTrianglesPerLeaf += 2;
        }
        settingsList.back().accelTreeMaxDepth += 2;
        settingsList.back().maxTrianglesPerLeaf = 2;
    }
}

void compare(const Settings& settings)
{
    namespace fs = std::filesystem;
    using path = fs::path;
    std::cout << "-- Comparing Output --\n";

    std::vector<path> outputFiles = settings.getOutputFiles();
    for (const path& outputFile : outputFiles) {
        path compareFile = settings.getCompareFile(outputFile);
        if (fs::exists(compareFile)) {
            ImageDiff imgDiff = Engine::diffImages(outputFile, compareFile);

            bool match = imgDiff.diffs.size() == 0;
            std::cout << "comparing " << outputFile << " and " << compareFile << std::endl;
			if (match) {
                std::cout << "both files are identical\n";
			}
            else {
                std::cout << "NO MATCH\n";
                imgDiff.writeToFile(settings.getDiffFile(outputFile));
            }
        }
        else {
            std::cout << outputFile << " has no corresponding compare file: " << compareFile.string() << "; skipping" << std::endl;
        }
    }
}

/* @brief Read settings and launch engine */
int launch()
{
    std::vector<Settings> settingsList = { Settings::load("settings.json") };

#ifdef NDEBUG
    if (settingsList.front().debugPixel) {
        throw std::runtime_error("Turn on Debug to debug pixels");
    }
#endif

    // addBenchmarkingPermutations(settingsList); // uncomment for benchmarking

    for (Settings& settings : settingsList) {
        std::cout << "Running iteration " << settings.iterationName() << std::endl;
        logSettingsIteration(settings);
        Engine engine{ settings };
		engine.runAllScenes();
		compare(settings);
    }


    if (settingsList.size() > 1) {
        std::cout << "\n\n\nGBestSettings:\n\n\n";
        std::cout << GBestSettings << std::endl;
    }

    return 0;
}

/* @brief Make relative paths (settings.json, sceneLibrary/, out/) independent of where the exe was launched from.
*  Walks up from the exe's folder to the first folder that holds settings.json, either directly
*  (exe deployed next to its data) or in main-win-console/ (exe in the solution's x64/<Config>/ folder). */
void enterDataDirectory()
{
    namespace fs = std::filesystem;
    wchar_t exePath[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    if (length == 0 || length == MAX_PATH) {
        throw std::runtime_error("Could not determine the executable's path");
    }

    for (fs::path dir = fs::path(exePath).parent_path(); !dir.empty(); dir = dir.parent_path()) {
        for (const fs::path& candidate : { dir, dir / "main-win-console" }) {
            if (fs::exists(candidate / "settings.json")) {
                fs::current_path(candidate);
                return;
            }
        }
        if (dir == dir.root_path()) {
            break;
        }
    }
    throw std::runtime_error("settings.json not found next to the executable or in any parent folder");
}

/* @brief As a console application, propagates all exceptions */
int main()
{
    enterDataDirectory();
    // Loop main to detect race conditions, memory leaks, etc.
    size_t runTimes = 1;
    for (size_t i = 0; i < runTimes; ++i) {
        launch();
    }

}


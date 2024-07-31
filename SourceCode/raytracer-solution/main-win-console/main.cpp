/* Main entry for Windows console. Reads settings and starts the engine */
#include <iostream>
#include <fstream>

#include "include/Engine.h"
#include "include/Settings.h"

/* @brief Output benchmarking information for a settings permutation */ 
void logSettingsIteration(const Settings& settings) {
    std::ofstream logFile;
    logFile.open("out/" + settings.iterationName() + ".json", std::ios::out);
    logFile << settings.toString() << std::endl;
    std::cout << settings.toString() << std::endl;
    logFile.close();
}

/* @brief Add extra parameters */ 
void addSettingsPermutations(std::vector<Settings>& settingsList)
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

/* @brief Read settings and launch engine */
int launch()
{
    std::vector<Settings> settingsList = { Settings::load("settings.json") };
    // addSettingsPermutations(settingsList); // uncomment for benchmarking

    for (Settings& settings : settingsList) {
        std::cout << "Running iteration " << settings.iterationName() << std::endl;
        logSettingsIteration(settings);
        Engine engine{ settings };
        engine.runAllScenes();
    }

    if (settingsList.size() > 1) {
        std::cout << "\n\n\nGBestSettings:\n\n\n";
        std::cout << GBestSettings << std::endl;
    }

    return 0;
}

/* @brief As a console application, propagates all exceptions */
int main()
{
    try {
        // Loop main to detect race conditions, memory leaks, etc.
        size_t runTimes = 1;
        for (size_t i = 0; i < runTimes; ++i) {
            launch();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception caught: " << e.what() << std::endl;
        throw;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
        throw;
    }

}


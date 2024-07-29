#include <iostream>
#include <fstream>

#include "Engine.h"
#include "Settings.h"

void logSettingsIteration(const Settings& settings) {
    std::ofstream logFile;
    logFile.open("out/" + settings.iterationName() + ".json", std::ios::out);
    logFile << settings.toString() << std::endl;
    std::cout << settings.toString() << std::endl;
    logFile.close();
}

int launch() 
{
    std::vector<Settings> settingsList = {Settings::load("settings.json")};

    // Manually specify multiple iterations based on settings file

    //settingsList.back().accelTreeMaxDepth = 2;
    //settingsList.back().maxTrianglesPerLeaf = 2;

    //for (int i = 1; i < 30; ++i) {
    //    for (int j = 0; j < 30; ++j)
    //    {
    //        settingsList.push_back(settingsList.back());
    //        settingsList.back().settingsId += 1;
    //        settingsList.back().maxTrianglesPerLeaf += 2;
    //    }
    //        settingsList.back().accelTreeMaxDepth += 2;
    //        settingsList.back().maxTrianglesPerLeaf = 2;
    //}

    for (Settings& settings : settingsList) {
        std::cout << "Running iteration " << settings.iterationName() << std::endl;
        logSettingsIteration(settings);
        Engine rt {settings};
        rt.runAllScenes();
    }

    //std::cout << "\n\n\nGBestSettings:\n\n\n";
    //std::cout << GBestSettings << std::endl;
    return 0;
}

int main()
{
    // loop main to detect race conditions, memory leaks, etc.
    size_t runTimes = 1;
    for(size_t i = 0; i < runTimes; ++i) {
        launch();
    }
}


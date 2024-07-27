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

int main() 
{
    std::vector<Settings> settingsList = {Settings::load("settings.json")};

    // Manually specify multiple iterations based on settings file

    for (int i = 1; i < 15; ++i) {
        settingsList.push_back(settingsList.back());
        settingsList.back().settingsId += 1;
        settingsList.back().accelTreeMaxDepth += 2;
    }

    for (Settings& settings : settingsList) {
        std::cout << "Running iteration " << settings.iterationName() << std::endl;
        logSettingsIteration(settings);
        Engine rt {settings};
        rt.runAllScenes();
    }

    return 0;
}
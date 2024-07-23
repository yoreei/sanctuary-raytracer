#include <iostream>
#include <fstream>

#include "Raytracer.h"
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
    std::vector<Settings> settingsList = Settings::load("settings.json");

    // Manually specify multiple iterations based on settings file
    settingsList.push_back(settingsList.back());
    Settings& lastSettings = settingsList.back();
    lastSettings.forceNoAccelStructure = true;
    lastSettings.settingsId += 1;

    for (Settings& settings : settingsList) {
        std::cout << "Running iteration " << settings.iterationName() << std::endl;
        logSettingsIteration(settings);
        Raytracer rt {settings};
        rt.run();
    }

    return 0;
}
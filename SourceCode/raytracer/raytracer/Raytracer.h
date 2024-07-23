#pragma once
#include <string>
#include <vector>
#include "Settings.h"

class Metrics;
class Scene;
class Image;
class Raytracer {
public:
    Raytracer(const Settings& settings) : settings(settings) {}
    void writeFile(const std::string& filename, const std::string& data);
    void runScene(const std::string& filePath, const std::string& sceneName);
    int run();

private:
    void logFrame(const std::string& filenameNoExt, const Scene& scene, const Image& image);
    const Settings& settings;
};
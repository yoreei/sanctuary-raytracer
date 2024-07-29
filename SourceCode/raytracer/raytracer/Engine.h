#pragma once
#include <string>
#include <vector>
#include "Settings.h"
#include "Image.h"
#include "Renderer.h"

class Metrics;
class Scene;
class Image;
class Engine {
public:
    Engine(const Settings& settings) : settings(settings) {}
    void loadScene(const std::string& filePath, const std::string& sceneName);
    int runAllScenes();
    void tick();

private:
    void writeFrame() const;
    void cleanFrame();
    void writeFile(const std::string& filename, const std::string& data) const;

    // Renderer output
    Image image {};
    /* May contain normal buffer, z-buffer, reflection & refraction buffer, depending on settings */
    std::vector<Image> auxImages {};

    const Settings& settings;
    Scene scene {"scene", &settings};
    Renderer renderer {&settings, &scene, &image, &auxImages};
};
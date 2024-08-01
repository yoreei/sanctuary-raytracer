#pragma once
#include <string>
#include <vector>

#include "include/Settings.h"
#include "include/Image.h"
#include "include/Renderer.h"

class Metrics;
class Scene;
class Image;
class Engine {
public:
    Engine(const Settings& settings) : settings(settings) {}
    /* @brief Load all scenes, and `tick`s them until GEndFrame */
    int runAllScenes();
    /* @brief Read scene file and prepare for `tick()` */
    void loadScene(const std::string& filePath, const std::string& sceneName);
    /* @brief Render the loaded scene from loadScene */
    void tick();

private:
    void writeFrame() const;
    void cleanFrame();
    void writeFile(const std::string& filename, const std::string& data) const;

    // Renderer output
    Image image{};
    /* Auxiliary output images. To be used for deferred shading, debugging, etc. */
    std::vector<Image> auxImages {};

    const Settings& settings;
    Scene scene{ "scene", &settings };
    Renderer renderer{ &settings, &scene, &image, &auxImages };
};
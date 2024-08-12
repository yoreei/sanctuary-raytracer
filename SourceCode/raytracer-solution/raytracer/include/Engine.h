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
    void loadScene(const std::filesystem::path& filePath);
    /* @brief Render the loaded scene from loadScene */
    void startTick();
    static std::vector<Vec2<size_t>> diffImages(std::filesystem::path img1, std::filesystem::path img2);

private:
    void writeFrame() const;
    void cleanFrame();
    void writeFile(const std::string& filename, const std::string& data) const;
    std::vector<std::filesystem::path> getScenesToLoad() const;

    // Renderer output
    Image image{};
    /* Auxiliary output images. To be used for deferred shading, debugging, etc. */
    std::vector<Image> auxImages {};

    Settings settings;
    Scene scene{ "scene", &settings };
    Renderer renderer{ &settings, &scene, &image, &auxImages };
};
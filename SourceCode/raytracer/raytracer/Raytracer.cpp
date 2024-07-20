#pragma warning( disable : 4365 )

#include <fstream>
#include <cstdint>
#include <memory>
#include <string>
#include <filesystem>

#include "Raytracer.h"
#include "Animator.h"
#include "Image.h"
#include "CRTTypes.h"
#include "Triangle.h"
#include "Camera.h"
#include "Scene.h"
#include "Metrics.h"
#include "CRTSceneLoader.h"
#include "Renderer.h"
#include "Settings.h"

#include "GlobalDebugData.h"

namespace fs = std::filesystem;

void Raytracer::writeFile(const std::string& filename, const std::string& data) {
    std::ofstream ppmFileStream(filename, std::ios::out | std::ios::binary);
    ppmFileStream.write(data.c_str(), data.size());
    ppmFileStream.close();
}

void Raytracer::runScene(const std::string& sceneName, Metrics& metrics)
{
    // TODO: Make these configurable
    bool bWritePng = true;
    bool bWriteBmp = true;
    std::cout << "Running scene: " << sceneName << std::endl;
    Image image {};
    std::shared_ptr<Scene> scene = std::make_shared<Scene>(sceneName);
    Renderer renderer {settings, scene};
    
    CRTSceneLoader::loadCrtscene(settings, sceneName + ".crtscene", *scene, image) ? void() : exit(1);
    auto truck = MoveAnimation::Make(MoveType::Truck, 3, 0, 24);
    //scene->animator.addAnimation(scene->camera, truck);
    fs::create_directories("out/" + sceneName);
    //image = Image(1280, 720); // Make rendering time shorter for quick testing
    //image = Image(300, 200); // Make rendering time shorter for quick testing
    std::vector<Image> imageComponents {};

    do {
        renderer.renderScene(image, imageComponents);

        std::string filename = "out/" + sceneName + "/" + std::to_string(scene->animator.getCurrentFrame());

        std::cout << filename << std::endl << scene->metrics.toString() << std::endl;
        std::cout << "---" << std::endl;
        std::cout << GlobalDebug::toString();
        std::cout << "---" << std::endl;

        image.writeImage(filename, bWritePng, bWriteBmp);
        for (size_t i = 0; i < imageComponents.size(); i++) {
            imageComponents[i].writeImage(filename + "_depth_" + std::to_string(i), bWritePng, bWriteBmp);
        }
    } while (scene->animator.update());
    metrics = scene->metrics;
}

int Raytracer::run()
{
    using path = std::filesystem::path;
    std::vector<Metrics> metricsList = {};
    for (const auto& entry : fs::directory_iterator(settings.inputDir)) {
        if (path ext = entry.path().extension(); ext != ".crtscene") {
            continue;
        }

        std::string sceneName = entry.path().filename().string();
        sceneName = sceneName.substr(0, sceneName.find(".crtscene"));
        Metrics metrics {};
        runScene(sceneName, metrics);
        metricsList.push_back(metrics);
    }
    writeMetrics(metricsList);

    return 0;
}

void Raytracer::writeMetrics(const std::vector<Metrics>& metricsList) {
    std::ofstream metricsFileStream("out/metrics.txt", std::ios::out);
    for (const auto& metrics : metricsList) {
        metricsFileStream << metrics.toString() << std::endl << std::endl;
    }
}

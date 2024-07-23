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

void Raytracer::runScene(const std::string& filePath, const std::string& sceneName)
{
    std::cout << "Running scene: " << sceneName << std::endl;
    Image image {};
    std::shared_ptr<Scene> scene = std::make_shared<Scene>(sceneName, settings);
    Renderer renderer {settings, scene};
    
    CRTSceneLoader::loadCrtscene(settings, filePath, *scene, image) ? void() : exit(1);
    auto truck = MoveAnimation::Make(MoveType::Truck, 3, 0, 24);
    //scene->animator.addAnimation(scene->camera, truck);
    std::string directory = "out/" + settings.iterationName() + "/" + settings.projectDir + "/" + sceneName;
    fs::create_directories(directory);
    std::vector<Image> imageComponents {};

    do {
        size_t frameNumber = scene->animator.getCurrentFrame();
        renderer.renderScene(image, imageComponents);

        image.writeImage(settings.framePathNoExt(sceneName, frameNumber), settings.bWritePng, settings.bWriteBmp);
        for (size_t i = 0; i < imageComponents.size(); i++) {
            imageComponents[i].writeImage(settings.framePathNoExt(sceneName, frameNumber) + "_depth_" + std::to_string(i), settings.bWritePng, settings.bWriteBmp);
        }
        logFrame(settings.framePathNoExt(sceneName, frameNumber), *scene, image);

    } while (scene->animator.update());

    // write last frame metrics:

}

int Raytracer::run()
{
    using path = std::filesystem::path;
    using dir =  std::filesystem::directory_entry;
    std::vector<path> scenePaths = {};

    if (settings.loadEntireProject()) {
        for (const dir& entry : fs::directory_iterator(settings.sceneLibraryDir + "/" + settings.projectDir)) {
            if (path ext = entry.path().extension(); ext == ".crtscene") {
                scenePaths.push_back(entry.path());
            }
        }
    }
    else {
        for (const std::string& sceneFile : settings.targetScenes) {
            scenePaths.push_back(path(settings.sceneLibraryDir + "/" + settings.projectDir + "/" + sceneFile));
        }
    }

    for (const auto& scenePath : scenePaths) {
        std::string sceneName = scenePath.filename().string();
        sceneName = sceneName.substr(0, sceneName.find(".crtscene"));
        runScene(scenePath.string(), sceneName);
    }

    return 0;
}

void Raytracer::logFrame(const std::string& filenameNoExt, const Scene& scene, const Image& image) {
    std::ofstream fileStream(filenameNoExt + ".log", std::ios::out);
    std::string metricsString = scene.metrics.toString();
    std::string globalDebugString = GlobalDebug::toString();

    fileStream << metricsString << std::endl;
    fileStream << std::endl;
    fileStream << globalDebugString << std::endl;
    fileStream << std::endl;

    std::cout << metricsString << std::endl;
    std::cout << std::endl;
    std::cout << globalDebugString << std::endl;
    std::cout << std::endl;

    if (settings.debugPixel) {
        std::cout << "debugPixel: " << image(settings.debugPixelX, settings.debugPixelY) << std::endl;

        fileStream << "debugPixel: " << image(settings.debugPixelX, settings.debugPixelY) << std::endl;
    }

}

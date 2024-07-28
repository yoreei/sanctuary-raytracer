#pragma warning( disable : 4365 )

#include <fstream>
#include <cstdint>
#include <memory>
#include <string>
#include <filesystem>

#include "Engine.h"
#include "Image.h"
#include "CRTTypes.h"
#include "Triangle.h"
#include "Camera.h"
#include "Scene.h"
#include "Metrics.h"
#include "CRTSceneLoader.h"
#include "Renderer.h"
#include "Settings.h"
#include "Scripts.h"
#include "Globals.h"
#include "Utils.h"

namespace fs = std::filesystem;

void Engine::writeFile(const std::string& filename, const std::string& data) const {
    std::ofstream ppmFileStream(filename, std::ios::out | std::ios::binary);
    ppmFileStream.write(data.c_str(), data.size());
    ppmFileStream.close();
}

void Engine::tick()
{
    std::string directory = settings.projectPath() + "/" + scene.fileName;
    fs::create_directories(directory);

    while (GFrameNumber <= GEndFrame) {
        std::cout << ">>> Frame: " << GFrameNumber << std::endl;
        Scripts::onTick(scene);
        scene.updateAnimations();
        renderer.render();
        writeFrame();
        ++GFrameNumber;
        auto summedMetrics = GSceneMetrics.toJson();

        uint64_t triInt = Utils::jsonGetDefault<uint64_t>(summedMetrics["counters"], "TriangleIntersection", 0);
            if (triInt < GBestTriangleIntersect) {
                GBestSettings = summedMetrics.dump(4);
        }
    }

}

void Engine::loadScene(const std::string& filePath, const std::string& sceneName) {
    std::cout << ">> Loading scene: " << sceneName << std::endl;
    GResetGlobals();
    scene.fileName = sceneName;
    CRTSceneLoader::loadCrtscene(settings, filePath, scene, image) ? void() : exit(1);
    Scripts::onSceneLoaded(scene);
    std::cout << ">> Scene " << sceneName << " loaded\n";
}

int Engine::runAllScenes()
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
        GSceneMetrics.clear();
        loadScene(scenePath.string(), sceneName);
        tick();

    }

    return 0;
}

void Engine::writeFrame() const {
    std::string framePathNoExt = settings.framePathNoExt(scene.fileName, GFrameNumber);
    image.writeImage(framePathNoExt, settings.bWritePng, settings.bWriteBmp);
    for (size_t i = 0; i < auxImages.size(); i++) {
        auxImages[i].writeImage(framePathNoExt + "_depth_" + std::to_string(i), settings.bWritePng, settings.bWriteBmp);
    }

    std::ofstream fileStream(framePathNoExt + ".log", std::ios::out);
    std::string metricsString = GSceneMetrics.toString();
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
    std::cout <<std::endl;

}


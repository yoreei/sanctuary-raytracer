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

namespace fs = std::filesystem;

void Raytracer::writeFile(const std::string& filename, const std::string& data) {
    std::ofstream ppmFileStream(filename, std::ios::out | std::ios::binary);
    ppmFileStream.write(data.c_str(), data.size());
    ppmFileStream.close();
}

void Raytracer::runScene(const std::string& sceneName)
{
    Image image {};
    Scene scene {};
    Animator animator {scene, 0};
    Renderer renderer {};
    
    CRTSceneLoader::loadCrtscene("scenes/" + sceneName + ".crtscene", scene, image) ? void() : exit(1);
    fs::create_directories("out/" + sceneName);
    image = Image(1280, 720); // Make rendering time shorter for quick testing
    //image = Image(300, 200); // Make rendering time shorter for quick testing

    do {
        scene.metrics = Metrics {}; // reset
        renderer.renderScene(scene, image);

        std::string filename = "out/" + sceneName + "/" + std::to_string(animator.getCurrentFrame()) + ".png";

        std::cout << filename << std::endl << scene.metrics.toString() << std::endl;
        std::cout << scene.materials[0] << std::endl;
        std::cout << "---" << std::endl;

        image.writeToPng(filename);
    } while (animator.update());
}

int Raytracer::run()
{
    for (const auto& entry : fs::directory_iterator("scenes")) {
        if (auto ext = entry.path().extension(); ext != ".crtscene") {
            continue;
        }

        std::string sceneName = entry.path().filename().string();
        sceneName = sceneName.substr(0, sceneName.find(".crtscene"));
        runScene(sceneName);
    }

    return 0;
}

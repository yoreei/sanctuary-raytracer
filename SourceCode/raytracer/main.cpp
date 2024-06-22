#include <fstream>
#include <cstdint>
#include <memory>
#include <string>

#include "Animator.h"
#include "Image.h"
#include "CRTTypes.h"
#include "Triangle.h"
#include "Camera.h"
#include "Scene.h"
#include "Renderer.h"
#include "hw4.h"
#include "HardCodedScenes.h"

void writeFile(const std::string& filename, const std::string& data) {
    std::ofstream ppmFileStream(filename, std::ios::out | std::ios::binary);
    ppmFileStream.write(data.c_str(), data.size());
    ppmFileStream.close();
}

int main()
{
    Image image = {800, 600};
    Scene scene {};
    Animator animator {scene, 0};
    Scenes::loadHw6task3and4(scene, animator);

    do {
        Renderer renderer{}; // reset metrics
        renderer.renderScene(scene, image);

        std::string filename = "out/" + std::to_string(animator.getCurrentFrame()) + ".png";
        std::cout << filename << std::endl << renderer.metrics.toString() << std::endl;
        std::cout << "---" << std::endl;

        image.writeToPng(filename);
        // writeFile(filename, image.toPpmString());
    } while (animator.update());

    return 0;
}

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

void loadScene(Scene& scene)
{
    float fov = 90.f;
    Vec3 camPos = { 0.f,0.f,0.f };
    Vec3 dir = { 0.f, 0.f, -1.f };
    dir = dir.normalize();
    Vec3 right = Matrix3x3::pan(90.f) * dir;
    Vec3 up = Matrix3x3:: tilt(90.f) * dir;

    std::cout << "Cam: [ " << right << " " << up << " " << dir << " ] " << std::endl;

    Matrix3x3 camMat = Matrix3x3::fromCols(right, up, dir);
    scene.camera = Camera{fov, camPos, camMat};
    scene.camera.addAnimation<RotateAnimation>(Matrix3x3::roll(360), 72);

    std::vector<Triangle> task1 =
    { {
        Vec3{-1.75f, -1.75f, -3.f},
        Vec3{1.75f, -1.75f, -3.f},
        Vec3{0.f, 1.75f, -3.f},
    } };

    // Triangles for hw5 task2
    // alls vertices even with the image plane
    Triangle tri_even
    {
        Vec3{-1.f, -1.f, -1.f},
        Vec3{1.f, -1.f, -1.f},
        Vec3{0.f, 1.f, -1.f},
    };


    // Bottom vert deeper
    Triangle tri1
    {
        Vec3{-1.f, -1.f, -1.f},
        Vec3{-0.8f, -1.f, -1.f},
        Vec3{-0.9f, -0.8f, -1.5f}
    };

    // right vert deeper
    Triangle tri2
    {
        Vec3{-1.f, -0.8f, -1.f},
        Vec3{-1.f, -1.f, -1.f},
        Vec3{-0.8f, -0.9f, -1.5f},
    };

    // top vert deeper
    Triangle tri3
    {
        Vec3{0.8f,1.f,-1.f},
        Vec3{0.9f,0.8f,-1.5f},
        Vec3{1.f,1.f,-1.f},
    };

    // Task 3
    std::vector<Triangle> task3 = {
        {
            Vec3{-1.f, -1.f, -1.f},
            Vec3{1.f, -1.f, -1.f},
            Vec3{-1.f, 1.f, -1.f},
        },
        {
            Vec3{1.f, -1.f, -1.f},
            Vec3{1.f, 1.f, -1.f},
            Vec3{-1.f, 1.f, -1.f},
        }
    };

    // Task 4
    std::vector<Triangle> foxShape = {
        {
            Vec3{-1.f, -1.f, -2.f},
            Vec3{1.f, -1.f, -2.f},
            Vec3{0.f, 1.f, -2.3f},
        },
        {
            Vec3{-1.f, -1.f, -2.f},
            Vec3{-0.9f, -1.3f, -2.1f},
            Vec3{-0.8f, -1.f, -2.f},
        },
        {
            Vec3{0.8f, -1.f, -2.f},
            Vec3{0.9f, -1.3f, -2.1f},
            Vec3{1.f, -1.f, -2.f},
        },
    };

    scene.triangles = task1;
    std::cout << "0: " << foxShape[0].normal().toString() << std::endl
        << "1: " << foxShape[1].normal().toString() << std::endl
        << "2: " << foxShape[2].normal().toString() << std::endl;
}

void writeFile(const std::string& filename, const std::string& data) {
    std::ofstream ppmFileStream(filename, std::ios::out | std::ios::binary);
    ppmFileStream.write(data.c_str(), data.size());
    ppmFileStream.close();
}

int main()
{
    Image image = {800, 600};
    Scene scene {};
    loadScene(scene);
    Animator animator = Animator{scene};
    Renderer renderer{};
    renderer.renderScene(scene, image);
    std::cout << renderer.metrics.toString();
    writeFile("crt_output_image.ppm", image.toPpmString());

    return 0;
}

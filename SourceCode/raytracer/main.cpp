#include <fstream>
#include <cstdint>
#include <memory>
#include <string>

#include "CRTTypes.h"
#include "Renderer.h"
#include "Scene.h"
#include "hw4.h"
#include "Triangle.h"

/// Output image resolution
static const int maxColorComponent = 255;

void save_ppm(Buffer2D& buf)
{
    std::ofstream ppmFileStream("crt_output_image.ppm", std::ios::out | std::ios::binary);
    ppmFileStream << "P3\n"; ppmFileStream << buf.width << " " << buf.height << "\n";
    ppmFileStream << maxColorComponent << "\n";

    for (int rowIdx = 0; rowIdx < buf.height; ++rowIdx) {
        for (int colIdx = 0; colIdx < buf.width; ++colIdx) {
            const Color& Color = buf.data[rowIdx * buf.width + colIdx];
            ppmFileStream << (int)Color.r << " " << (int)Color.g << " " << (int)Color.b << "\t";
        }
        ppmFileStream << "\n";
    }

    ppmFileStream.close();
}

void loadScene(Scene& scene, uint16_t width, uint16_t height)
{
    float fov = 90.f;
    Vec3 camPos = {0.f,0.f,0.f};
    Matrix3x3 camMat = Matrix3x3{ {
    1, 0, 0,
    0, 1, 0,
    0, 0, -1
    } };
    scene.camera = Camera{width, height, fov, camPos, camMat};

    Triangle tri_task1
    {
        Vec3{-1.75f, -1.75f, -3.f},
        Vec3{1.75f, -1.75f, -3.f},
        Vec3{0.f, 1.75f, -3.f},
    };

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

    scene.triangles = foxShape;
}

int main()
{
    uint16_t width = 800, height = 600;
    Buffer2D buf = {width, height};
    Scene scene;
    loadScene(scene, width, height);
    Renderer renderer = Renderer{&scene};
    renderer.render(buf);
    save_ppm(buf);
    return 0;
}

#pragma once
#include "Camera.h"
#include "Settings.h"
#include "Triangle.h"

struct SceneObject {
    Vec3 pos{ 0.f, 0.f, 0.f };
    Vec3 dir{ 0.f, 0.f, -1.f };
    std::vector<Animation> animations {};
}

struct Scene
{

    Camera camera;
    Settings settings;
    std::vector<Triangle> triangles;
    Color bgColor = Color{0, 0, 0};
};
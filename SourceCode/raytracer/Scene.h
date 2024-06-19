#pragma once
#include "Camera.h"
#include "Settings.h"
#include "Triangle.h"

struct Scene
{
    Camera camera;
    Settings settings;
    std::vector<Triangle> triangles;
    Color bgColor = Color{0, 0, 0};
};
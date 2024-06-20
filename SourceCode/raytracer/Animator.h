#pragma once
#include "Scene.h"
#include "Animation.h"

class Animator
{
public:
    Animator(Scene& scene) : scene(scene) {}

    void update()
    {
        for (auto& animation : scene.camera.animations) {
            animation->apply(scene.camera);
        }
    }

private:
    Scene& scene;
};


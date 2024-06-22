#pragma once
#include "Scene.h"
#include "Animation.h"

class Animator
{
public:
    Animator(Scene& scene, int endFrame) : scene(scene), endFrame(endFrame) {}

    /*
    * Return true until all animations are finished
    */
    bool update()
    {
        // TODO: Improve implementation, not optimal

        for (auto& animation : scene.camera.animations) {
            if (currentFrame >= animation->getStartFrame() && currentFrame < animation->getEndFrame()) {
               animation->apply(scene.camera);
            }
        }

        ++currentFrame;
        return currentFrame < endFrame;
    }

    auto getCurrentFrame() const
    {
        return currentFrame;
    }

    int currentFrame = 0;
    int endFrame = 0;
private:
    Scene& scene;
};


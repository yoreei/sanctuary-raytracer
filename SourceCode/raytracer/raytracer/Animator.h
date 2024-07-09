#pragma once
#include <memory>

class Animation;
class SceneObject;
class Scene;
class Animator
{
public:
    Animator(Scene& scene) : scene(scene) {}

    /*
    * Return true until all animations are finished
    */
    bool update();

    int getCurrentFrame() const;

    void addAnimation(SceneObject& object, const std::shared_ptr<Animation>& animation);

    int currentFrame = 0;
    int endFrame = 0;
private:
    Scene& scene;
};


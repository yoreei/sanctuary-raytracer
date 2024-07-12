#pragma warning( disable : 4365 )

#include <memory>

#include "Animator.h"
#include "Animation.h"
#include "Scene.h"
#include "SceneObject.h"


/*
* Return true until all animations are finished
*/

bool Animator::update()
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

int Animator::getCurrentFrame() const
{
    return currentFrame;
}

void Animator::addAnimation(SceneObject& object, const std::shared_ptr<Animation>& animation)
{
    object.animations.push_back(animation);
    endFrame = std::max(endFrame, animation->getEndFrame());
}

#pragma once
#include "Globals.h"

#include "CRTTypes.h"
#include "Scene.h"
#include "AnimationComponent.h"

namespace Scripts {

    void onSceneLoaded(Scene& scene);

    float easeInOutCubic(float t);

    float applyEasing(float t);

    void onTick(Scene& scene);

} // namespace Scripts

#pragma once
#include "include/Globals.h"

#include "include/CRTTypes.h"
#include "include/Scene.h"
#include "include/AnimationComponent.h"

namespace Scripts {

    void onSceneLoaded(Scene& scene);

    float easeInOutCubic(float t);

    float applyEasing(float t);

    void onTick(Scene& scene);

} // namespace Scripts

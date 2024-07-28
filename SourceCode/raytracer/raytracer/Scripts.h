#pragma once
#include "Globals.h"

#include "CRTTypes.h"
#include "Scene.h"
#include "AnimationComponent.h"

namespace Scripts {
    //void onSceneLoaded(Scene& scene) {
    //    scene;
    //    //GEndFrame = 0;
    //    //scene.camera.pos = {0.f, 14.f, 0.f};
    //    //scene.camera.lookAt({0.f, 0.f, 0.f});

    //}

    //void onSceneLoaded(Scene& scene) {
    //    scene;
        //float endFrame = 24;
        //GEndFrame = uint64_t(endFrame);

        //Camera cam0 = scene.camera;
        //cam0.pos = {0.f, 0.f ,0.f};
        //cam0.lookAt({0.f, 0.f, -1.f});

        //Camera cam1 = scene.camera;
        //cam1.pos = {0.f, 10.f ,0.f};
        //cam1.lookAt({0.f, -1.f, 0.f});

        //std::vector<Keyframe<Vec3>> dirKfs {
        //    {0.f, cam0.getDir()},
        //    {endFrame, cam1.getDir()},
        //};

        //std::vector<Keyframe<Vec3>> posKfs {
        //    {0.f, cam0.pos},
        //    {endFrame, cam1.pos},
        //};
        //scene.cameraAnimations[0] = AnimationComponent{};
        //scene.cameraAnimations[0].pos = AnimationCurve{posKfs};
        //scene.cameraAnimations[0].dir = AnimationCurve{dirKfs};
    //}

    void onSceneLoaded(Scene& scene) {
        scene;
        //float endFrame = 10.f;
        //GEndFrame = uint64_t(endFrame);

        //std::vector<Keyframe<Vec3>> dirKfs {
        //    {0.f, scene.camera.getDir()},
        //    {endFrame, {0.f, -1.f, 0.f}},
        //};

        //scene.cameraAnimations[0] = AnimationComponent{};
        //scene.cameraAnimations[0].dir = AnimationCurve{dirKfs};
    }


    void onTick(Scene& scene) {
        scene;

    }

} // namespace Scripts

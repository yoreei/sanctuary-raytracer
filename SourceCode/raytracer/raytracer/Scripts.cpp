#include "Scripts.h"

static float fps;
static float duration;
static float endFrame;
void Scripts::onSceneLoaded(Scene& scene) {
    scene;
    // set start frame
    GFrameNumber = 0;

    fps = 60; // Frames per second
    duration = 7.0f; // Duration of the animation in seconds
    endFrame = fps * duration;
    GEndFrame = uint64_t(endFrame);
}

float Scripts::easeInOutCubic(float t) {
    return t < 0.5f ? 4 * t * t * t : 1 - powf(-2 * t + 2, 3) / 2;
}

float Scripts::applyEasing(float t) {
    float easeDuration = 2.0f; // 3 seconds for easing in and out
    float middleStart = easeDuration / duration;
    float middleEnd = 1.0f - middleStart;

    if (t < middleStart) {
        float localT = t / middleStart;
        return easeInOutCubic(localT) * middleStart;
    }
    else if (t > middleEnd) {
        float localT = (t - middleEnd) / middleStart;
        return middleEnd + easeInOutCubic(localT) * middleStart;
    }
    else {
        return t;
    }
}

void Scripts::onTick(Scene& scene) {
    //if (scene.fileName == "day") {

    //    // Parameters for the circular path
    //    float startAngle = PI / 4.f; // (radians)
    //    float startRadius = 14.0f; // Radius of the circle
    //    float endRadius = 8.f;
    //    float startCamHeight = 8.f;
    //    float endCamHeight = -4.f;

    //    float t = GFrameNumber / endFrame; // Normalized time [0, 1]
    //    t = applyEasing(t); // slow down at start and end

    //    float angle = t * 2 * PI + startAngle; // Full circle (radians)
    //    float radius = startRadius * (1.f - t) + endRadius * t;
    //    float camHeight = startCamHeight * (1.f - t) + endCamHeight * t;

    //    Vec3 position = { radius * std::cos(angle), camHeight, radius * std::sin(angle) };

    //    scene.camera.pos = position;
    //    scene.camera.lookAt({ 0.f, 0.f, 0.f });
    //}

    if (scene.fileName == "day") {

        // Parameters for the circular path
        float startAngle = PI / 4.f; // (radians)
        float endAngle = 5 * PI / 4.f; // (radians)
        float startRadius = 16.0f; // Radius of the circle
        float endRadius = 7.f;
        float startCamHeight = 5.f;
        float endCamHeight = -4.f;

        float t = GFrameNumber / endFrame; // Normalized time [0, 1]
        t = applyEasing(t); // slow down at start and end

        float angle = startAngle * (1.f -t) + endAngle * t;
        float radius = startRadius * (1.f - t) + endRadius * t;
        float camHeight = startCamHeight * (1.f - t) + endCamHeight * t;

        Vec3 position = { radius * std::cos(angle), camHeight, radius * std::sin(angle) };

        scene.camera.pos = position;
        scene.camera.lookAt({ 0.f, 0.f, 0.f });
    }
    if (scene.fileName == "day2") {

        // Parameters for the circular path
        float startAngle = PI ; // (radians)
        float endAngle = PI / 4.f; // (radians)
        float startRadius = 14.0f; // Radius of the circle
        float endRadius = 8.f;
        float startCamHeight = 2.f;
        float endCamHeight = -4.f;

        float t = GFrameNumber / endFrame; // Normalized time [0, 1]
        t = applyEasing(t); // slow down at start and end

        float angle = startAngle * (1.f -t) + endAngle * t;
        float radius = startRadius * (1.f - t) + endRadius * t;
        float camHeight = startCamHeight * (1.f - t) + endCamHeight * t;

        Vec3 position = { radius * std::cos(angle), camHeight, radius * std::sin(angle) };

        scene.camera.pos = position;
        scene.camera.lookAt({ 0.f, 0.f, 0.f });
    }


    if (scene.fileName == "night") {
        fps = 1; // Frames per second
        duration = 5.0f; // Duration of the animation in seconds
        endFrame = fps * duration;
        GEndFrame = uint64_t(endFrame);

        // Parameters for the circular path
        float startFov = 30.f; // (degrees)
        float endFov = 90.f; // (degrees)

        float startAngle = -PI / 4.f; // (radians)
        float endAngle = -PI / 2.f; // (radians)
        float startRadius = 13.0f; // Radius of the circle
        float endRadius = 13.f;
        float startCamHeight = 8.f;
        float endCamHeight = 8.f;

        float t = GFrameNumber / endFrame; // Normalized time [0, 1]
        t = applyEasing(t); // slow down at start and end

        float angle = startAngle * (1.f -t) + endAngle * t;
        float radius = startRadius * (1.f - t) + endRadius * t;
        float camHeight = startCamHeight * (1.f - t) + endCamHeight * t;
        float fov = startFov * (1.f - t) + endFov * t;

        Vec3 position = { radius * std::cos(angle), camHeight, radius * std::sin(angle) };

        //fov;
        scene.camera.fov = fov;
        scene.camera.pos = position;
        scene.camera.lookAt({ 0.f, 0.f, 0.f });
    }

    if (scene.fileName == "night2") {
        fps = 1; // Frames per second
        duration = 8.0f; // Duration of the animation in seconds
        endFrame = fps * duration;
        GEndFrame = uint64_t(endFrame);

        // Parameters for the circular path
        float startFov = 30.f; // (degrees)
        float endFov = 90.f; // (degrees)

        Vec3 startLightPos  {-12.f, 5.f, 4.f};
        Vec3 endLightPos {-12.f, 5.f, -4.f};
        float lightEndFrame = 2.f * fps;
        Vec3 startLookAt {0.f, 3.f, 0.f};
        Vec3 endLookAt {0.f, 12.f, 0.f};
        float startAngle = PI ; // (radians)
        float endAngle = PI; // (radians)
        float startRadius = 13.0f; // Radius of the circle
        float endRadius = 9.f;
        float startCamHeight = 10.f;
        float endCamHeight = 4.f;

        float t = GFrameNumber / endFrame; // Normalized time [0, 1]
        float lightT = GFrameNumber / lightEndFrame;
        lightT = std::clamp(lightT, 0.f, 1.f);
        t = applyEasing(t); // slow down at start and end

        float angle = startAngle * (1.f -t) + endAngle * t;
        float radius = startRadius * (1.f - t) + endRadius * t;
        float camHeight = startCamHeight * (1.f - t) + endCamHeight * t;
        float fov = startFov * (1.f - t) + endFov * t;

        Vec3 lightPos = lerp(startLightPos, endLightPos, lightT);
        Vec3 lookAt = lerp(startLookAt, endLookAt, t);
        Vec3 position = { radius * std::cos(angle), camHeight, radius * std::sin(angle) };

        //fov;
        scene.lights[4].pos = lightPos;
        scene.camera.fov = fov;
        scene.camera.pos = position;
        scene.camera.lookAt(lookAt);
    }



    //if (scene.fileName == "night") {
    //    float fps = 1; // Frames per second
    //    float duration = 1.0f; // Duration of the animation in seconds
    //    float endFrame = fps * duration;
    //    GEndFrame = uint64_t(endFrame);

    //    // Parameters for the circular path
    //    float startFov = 30.f; // (degrees)
    //    float endFov = 90.f; // (degrees)
    //    float startAngle = PI / 4.f; // (radians)
    //    float endAngle = PI / 6.f; // (radians)
    //    float startDistance = 7.0f; // Radius of the circle
    //    float endDistance = 7.f;
    //    float startCamHeight = 0.f;
    //    float endCamHeight = 0.f;

    //    float t = GFrameNumber / endFrame; // Normalized time [0, 1]
    //    t = applyEasing(t, duration); // slow down at start and end

    //    float angle = t * endAngle + startAngle; // Full circle (radians)
    //    float distance = startDistance * (1.f - t) + endDistance * t;

    //    float camHeight = startCamHeight * (1.f - t) + endCamHeight * t;
    //    float fov = startFov * (1.f - t) + endFov * t;

    //    Vec3 position = { distance * std::cos(angle), camHeight, distance * std::sin(angle) };

    //    // scene.camera.fov = fov;
    //    fov;
    //    // scene.camera.pos = {startDistance * std::cos(startAngle), startCamHeight, startDistance * std::sin(startAngle) };
    //    scene.camera.pos = position;
    //    scene.camera.lookAt({ 3.f, 0.f, 0.f });
    //    //scene.camera.setDir({0.f, 0.f, 1.f});
    //    std::cout << "Scripts:\n";
    //    std::cout << scene.camera.mat.toString() << std::endl;
    //}


}

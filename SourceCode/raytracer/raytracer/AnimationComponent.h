#pragma once
#include <vector>

#include "CRTTypes.h"
#include "Globals.h"

template <typename T>
struct Keyframe {
    float time;
    T value;
};

template <typename T>
struct AnimationCurve {
    std::vector<Keyframe<T>> keyframes;

    // Interpolates the value based on the current time
    void evaluate(T& outValue) const {
        if (keyframes.empty()) {
            return;
        }

        if (GFrameNumber <= keyframes.front().time) { outValue = keyframes.front().value; return; }
        if (GFrameNumber >= keyframes.back().time) { outValue = keyframes.back().value; return; }

        for (size_t i = 0; i < keyframes.size() - 1; ++i) {
            if (GFrameNumber >= keyframes[i].time && GFrameNumber < keyframes[i + 1].time) {
                float t = (GFrameNumber - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i + 1].time);
                outValue = keyframes[i].value * (1 - t) + keyframes[i + 1].value * t;
                return;
            }
        }

        throw std::runtime_error("should not reach this");
    }
};

struct AnimationComponent {
    AnimationCurve<Vec3> pos {}; // For SceneObject
    AnimationCurve<Vec3> rotation {}; // For SceneObject
    AnimationCurve<Matrix3x3> mat {}; // For Camera
    AnimationCurve<float> intensity {}; // For lights
    AnimationCurve<float> fieldOfView {}; // For camera
};

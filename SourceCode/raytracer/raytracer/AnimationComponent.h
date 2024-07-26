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

    /* Interpolates the value based on the current time (GFrameNumber) 
    *  Used where direct replacement of the object value is sufficient
    */
    void evaluateLerp(T& outValue) const {
        if (keyframes.empty()) {
            return;
        }

        if (GFrameNumber <= keyframes.front().time) { outValue = keyframes.front().value; return; }
        if (GFrameNumber >= keyframes.back().time) { outValue = keyframes.back().value; return; }

        for (size_t i = 0; i < keyframes.size() - 1; ++i) {
            if (GFrameNumber >= keyframes[i].time && GFrameNumber < keyframes[i + 1].time) {
                const float& startTime = keyframes[i].time;
                const float& stopTime = keyframes[i + 1].time;
                const T& startValue = keyframes[i].value;
                const T& stopValue = keyframes[i + 1].value;

                float lerpFactor = (GFrameNumber - startTime) / (stopTime - startTime);
                outValue = startValue * (1 - lerpFactor) + stopValue * lerpFactor;
                return;
            }
        }

        throw std::runtime_error("should not reach this");
    }

    void evaluateSlerp(Vec3& outValue) const {
        if (keyframes.empty()) {
            return;
        }

        if (GFrameNumber <= keyframes.front().time) { outValue = keyframes.front().value; return; }
        if (GFrameNumber >= keyframes.back().time) { outValue = keyframes.back().value; return; }

        for (size_t i = 0; i < keyframes.size() - 1; ++i) {
            if (GFrameNumber >= keyframes[i].time && GFrameNumber < keyframes[i + 1].time) {
                const float& startTime = keyframes[i].time;
                const float& stopTime = keyframes[i + 1].time;
                const Vec3& startValue = keyframes[i].value;
                const Vec3& stopValue = keyframes[i + 1].value;

                float t = (GFrameNumber - startTime) / (stopTime - startTime);
                outValue = slerp(startValue, stopValue, t);
                return;
            }
        }

        throw std::runtime_error("should not reach this");
    }
};


struct AnimationComponent {
    AnimationCurve<Vec3> pos {}; // For SceneObject
    AnimationCurve<Vec3> rotation {}; // For SceneObject
    AnimationCurve<Vec3> dir {}; // For SceneObject
    AnimationCurve<float> intensity {}; // For lights
    AnimationCurve<float> fieldOfView {}; // For camera
};

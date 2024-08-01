#pragma once
#include <vector>

#include "include/CRTTypes.h"
#include "include/Globals.h"

template <typename T>
struct Keyframe {
    float time;
    T value;
};

/* @brief Describes a curve (think position, rotation, intensity, etc) that can be animated
*  key points in the animation are stored as keyframes. AnimationCurve can interpolate between keyframes */
template <typename T>
struct AnimationCurve {
    std::vector<Keyframe<T>> keyframes;

    /* @brief Linear interpolation based on the current time (GFrameNumber) */
    void evaluateLerp(T& outValue) const;

    /* @brief Spherical linear interpolation value based on the current time (GFrameNumber) */
    void evaluateSlerp(Vec3& outValue) const;
};

/* @brief Describes the animation of a SceneObject, Light, or Camera */
struct AnimationComponent {
    AnimationCurve<Vec3> pos {}; // For SceneObject
    AnimationCurve<Vec3> rotation {}; // For SceneObject
    AnimationCurve<Vec3> dir {}; // For SceneObject
    AnimationCurve<float> intensity {}; // For lights
    AnimationCurve<float> fieldOfView {}; // For camera
};

// Template Function Definitions:

template<typename T>
void AnimationCurve<T>::evaluateLerp(T& outValue) const {
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

template<typename T>
void AnimationCurve<T>::evaluateSlerp(Vec3& outValue) const {
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

            assert(outValue.isUnit());
            return;
        }
    }

    throw std::runtime_error("should not reach this");
}


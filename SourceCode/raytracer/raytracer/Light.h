#pragma once
#include <algorithm>
#include <numbers>

#include "CRTTypes.h"
#include "SceneObject.h"

class Scene;

class Light : public SceneObject {
public:
    Light () = default;
    Light(const Vec3& pos, float intensity)
        : SceneObject(pos, Matrix3x3()), intensity(intensity) {}
    Light(const Vec3& pos, float intensity, const Vec3& color)
        : SceneObject(pos, Matrix3x3()), intensity(intensity), color(color) {}
    /*
    * Output:vector representing RGB contribution of the light source
    */
    Vec3 lightContrib(const Scene& scene, const Vec3& point, const Vec3& normal) const;
    float intensity = 100;
    Vec3 color{ 1.f, 1.f, 1.f }; // TODO implement color
};

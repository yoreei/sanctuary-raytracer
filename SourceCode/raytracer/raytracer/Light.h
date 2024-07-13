#pragma once
#include <algorithm>
#include <numbers>

#include "CRTTypes.h"
#include "SceneObject.h"

class Scene;

class Light : public SceneObject {
public:
    Light(const Vec3& pos, float intensity, const Color& color)
        : SceneObject(pos, Matrix3x3()), intensity(intensity), color(color) {}
    /*
    * Output:vector representing RGB contribution of the light source
    */
    Vec3 lightContrib(const Scene& scene, const Vec3& point, const Vec3& normal) const;
private:
    float intensity;
    Color color; // TODO implement color
};

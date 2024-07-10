#include <cmath>

#include "Light.h"
#include "CRTTypes.h"
#include "Scene.h"
#include "Scene.h"

static constexpr float PI = static_cast<float>(std::numbers::pi);

Vec3 Light::lightContrib(const Scene& scene, const Vec3& point, const Vec3& normal) const {
    Vec3 lightDir = (pos - point);
    float sr = lightDir.length(); // Sphere Radius
    lightDir.normalize();
    if (scene.isOccluded(point, lightDir)) {
        scene.metrics.record("LIGHT_OCCLUDED");
        return Vec3{ 0.f, 0.f, 0.f };
    }

    float cosLaw = std::fabs(normal.dot(lightDir)); // fabs takes care of refractive materials
    float sa = 4 * PI * sr * sr; // Sphere Area
    scene.metrics.record("LIGHT_SUCCESS");
    float contrib = this->intensity * cosLaw / sa;
    return Vec3{ contrib, contrib, contrib };
    if (contrib > 1.f) {
        scene.metrics.record("LIGHT_CONTRIB_GREATER_THAN_ONE");
    }
}

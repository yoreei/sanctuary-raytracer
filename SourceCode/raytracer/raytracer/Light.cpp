#pragma warning( disable : 4365 )

#include <cmath>

#include "Light.h"
#include "CRTTypes.h"
#include "Scene.h"
#include "Scene.h"

static constexpr float PI = static_cast<float>(std::numbers::pi);

Vec3 Light::lightContrib(const Scene& scene, const Vec3& point, const Vec3& normal) const {
    if (scene.isOccluded(point, pos)) {
        return Vec3{ 0.f, 0.f, 0.f };
    }

    Vec3 lightDir = (pos - point);
    float sr = lightDir.length(); // Sphere Radius
    lightDir.normalize();
    float cosLaw = std::fabs(normal.dot(lightDir)); // fabs takes care of refractive materials
    float sa = 4 * PI * sr * sr; // Sphere Area
    float contrib = this->intensity * cosLaw / sa;
    return Vec3{ contrib, contrib, contrib };
}

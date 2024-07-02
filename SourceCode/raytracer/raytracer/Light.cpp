#include "Light.h"
#include "Scene.h"

static constexpr float PI = static_cast<float>(std::numbers::pi);

Vec3 Light::lightContrib(const Scene& scene, const Vec3& point, const Vec3& normal) const {
    Vec3 lightDir = (pos - point);
    float sr = lightDir.length(); // Sphere Radius
    lightDir = lightDir.normalize();
    float cosLaw = std::max(0.0f, normal.dot(lightDir));
    float sa = 4 * PI * sr * sr; // Sphere Area

    if (scene.isOccluded(point, lightDir)) {
        return Vec3{ 0.f, 0.f, 0.f };
    }

    float contrib = this->intensity * cosLaw / sa;
    return Vec3{ contrib, contrib, contrib };
}

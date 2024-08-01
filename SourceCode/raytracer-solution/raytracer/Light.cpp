#pragma warning( disable : 4365 )

#include <cmath>

#include "include/Light.h"
#include "include/CRTTypes.h"
#include "include/Scene.h"
#include "include/Scene.h"
#include "include/Globals.h"


Vec3 Light::lightContrib(const Scene& scene, const Vec3& point, const Vec3& normal) const {

    if (type == LightType::POINT) {
        Vec3 lightDir = (pos - point);
        float srSq = lightDir.lengthSquared(); // Sphere Radius Squared
        lightDir.normalize();
        float cosLaw = std::fabs(normal.dot(lightDir)); // fabs takes care of refractive materials
        float sa = 4 * PI * srSq; // Sphere Area
        Vec3 contrib = color * intensity * cosLaw / sa;
        if (std::max({contrib.x, contrib.y, contrib.z}) < 0.01f) {
            return Vec3{ 0.f, 0.f, 0.f };
        }

        if (scene.isOccluded(point, pos)) {
            return Vec3{ 0.f, 0.f, 0.f };
        }
        return contrib;
    }

    if (type == LightType::SUN) {
        Vec3 sunPos = point - direction * 1e+3f;
        if (scene.isOccluded(point, sunPos)) {
            return Vec3{ 0.f, 0.f, 0.f };
        }

        float cosLaw = std::fabs(normal.dot(direction)); // fabs takes care of the inside of transmissive materials
        Vec3 contrib = intensity * cosLaw * color;
        return contrib;
    }
    else { throw std::runtime_error("unknown light type"); }
}

LightType Light::lightTypeFromString(const std::string& str) {
    if (str == "sun") {
        return LightType::SUN;
    }
    else if (str == "point") {
        return LightType::POINT;
    }
    else {
        throw std::runtime_error("Unknown light type: " + str);
    }
}

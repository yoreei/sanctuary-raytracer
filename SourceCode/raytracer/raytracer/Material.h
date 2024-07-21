#pragma once
#include "CRTTypes.h"

class TraceHit;
class Scene;
class Material
{
    static constexpr float REFLECTIVITY = 0.5f;
public:
    enum Type
    {
        VOID, // No material
        DIFFUSE,
        CONSTANT,
        REFLECTIVE,
        REFRACTIVE,
        DEBUG_NORMAL,
        DEBUG_UV,
        DEBUG_BARY
    };

    Material() = default;
    Material(const Vec3& albedo, bool smoothShading, Type type) : albedo(albedo), smoothShading(smoothShading), type(type) {}

    Type type = Type::DIFFUSE;
    float reflectivity = REFLECTIVITY;
    float ior = 0.f;
    float transparency = 0.f;
    size_t textureIdx = 0;
    /* Whether light calculations ignore objects with this material */
    bool occludes = true;
    bool hasTexture = false;
    bool smoothShading = false;

    static Type TypeFromString(const std::string& type);

    static std::string StringFromType(const Material::Type& type);

    std::string toString() const;

    Vec3 getAlbedo(const Scene& scene, const TraceHit& hit) const;

    void setAlbedo(const Vec3& _albedo) { this->albedo = _albedo; }
private:
    Vec3 albedo {0.f, 1.f, 0.f};
};

inline std::ostream& operator<<(std::ostream& os, const Material& material) {
    os << material.toString();
    return os;
}

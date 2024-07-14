#pragma once
#include "CRTTypes.h"

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
        DEBUG_NORMAL
    };

    Material() = default;
    Material(const Vec3& albedo, bool smoothShading, Type type) : albedo(albedo), smoothShading(smoothShading), type(type) {}

    Vec3 albedo {0.f, 1.f, 0.f};
    Type type = Type::DIFFUSE;
    float reflectivity = REFLECTIVITY;
    float ior = 0.f;
    float transparency = 0.f;
    bool smoothShading = false;
    /* Whether light calculations ignore objects with this material */
    bool occludes = true;

    static Type TypeFromString(const std::string& type);

    static std::string StringFromType(const Material::Type& type);

    std::string toString() const;

};

inline std::ostream& operator<<(std::ostream& os, const Material& material) {
    os << material.toString();
    return os;
}

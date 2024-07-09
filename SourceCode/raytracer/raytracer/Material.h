#pragma once
#include "CRTTypes.h"

class Material
{
    const float REFLECTIVITY = 0.5f;
public:
    enum Type
    {
        VOID, // No material
        DIFFUSE,
        CONSTANT,
        REFLECTIVE,
        REFRACTIVE,
        DEBUG
    };

    Material() = default;
    Material(const Vec3& albedo, bool smoothShading, Type type) : albedo(albedo), smoothShading(smoothShading), type(type) {}

    Vec3 albedo {0.f, 1.f, 0.f};
    bool smoothShading = false;
    Type type = Type::DIFFUSE;
    float reflectivity = REFLECTIVITY;
    float ior = 0.f;
    float transparency = 0.f;

    static Type TypeFromString(const std::string& type);

    static std::string StringFromType(const Material::Type& type);

    std::string toString() const;

};

inline std::ostream& operator<<(std::ostream& os, const Material& material) {
    os << material.toString();
    return os;
}

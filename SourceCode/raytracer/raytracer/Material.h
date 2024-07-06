#pragma once
#include "CRTTypes.h"

class Material
{
public:
    enum Type
    {
        DIFFUSE,
        REFLECTIVE,
        REFRACTIVE
    };

    Material() = default;
    Material(const Vec3& albedo, bool smooth_shading, Type type) : albedo(albedo), smoothShading(smoothShading), type(type) {}

    Vec3 albedo {0.f, 1.f, 0.f};
    bool smoothShading = false;
    Type type = Type::DIFFUSE;

    static Type TypeFromString(const std::string& type);

    static std::string StringFromType(const Material::Type& type);

    std::string toString() const;
};


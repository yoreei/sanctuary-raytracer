#include "Material.h"

Material::Type Material::TypeFromString(const std::string& type)
{
    if (type == "diffuse")
    {
        return Type::DIFFUSE;
    }
    else if (type == "reflective")
    {
        return Type::REFLECTIVE;
    }
    else if (type == "refractive")
    {
        return Type::REFRACTIVE;
    }
    else
    {
        throw std::runtime_error("Unknown material type: " + type);
    }
}

std::string Material::StringFromType(const Material::Type& type)
{
    if (type == Type::DIFFUSE)
    {
        return "diffuse";
    }
    else if (type == Type::REFLECTIVE)
    {
        return "reflective";
    }
    else if (type == Type::REFRACTIVE)
    {
        return "refractive";
    }
    else
    {
        throw std::runtime_error("Unknown material type");
    }
}

std::string Material::toString() const
{
    return "Material{ albedo=" + albedo.toString() + " + smooth_shading=" + stringFromBool(smoothShading) + " + type=" + StringFromType(type) + " }";

}

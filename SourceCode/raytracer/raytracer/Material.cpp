#include "Material.h"

Material::Type Material::TypeFromString(const std::string& type)
{
    if (type == "void")
    {
        return Type::VOID;
    }
    else if (type == "diffuse")
    {
        return Type::DIFFUSE;
    }
    else if (type == "constant")
    {
        return Type::CONSTANT;
    }
    else if (type == "reflective")
    {
        return Type::REFLECTIVE;
    }
    else if (type == "refractive")
    {
        return Type::REFRACTIVE;
    }
    else if (type == "debug")
    {
        return Type::DEBUG;
    }
    else
    {
        throw std::runtime_error("Unknown material type: " + type);
    }
}

std::string Material::StringFromType(const Material::Type& type)
{
    if (type == Type::VOID)
    {
        return "void";
    }
    else if (type == Type::DIFFUSE)
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
    else if (type == Type::DEBUG)
    {
        return "debug";
    }
    else if (type == Type::CONSTANT)
    {
        return "constant";
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

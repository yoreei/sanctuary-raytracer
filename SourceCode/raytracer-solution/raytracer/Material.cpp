#pragma warning( disable : 4365 )

#include "include/Material.h"
#include "include/Scene.h"

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
    else if (type == "debug_normal")
    {
        return Type::DEBUG_NORMAL;
    }
    else if (type == "debug_uv")
    {
        return Type::DEBUG_UV;
    }
    else if (type == "debug_bary")
    {
        return Type::DEBUG_BARY;
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
    else if (type == Type::DEBUG_NORMAL)
    {
        return "debug_normal";
    }
    else if (type == Type::DEBUG_UV)
    {
        return "debug_uv";
    }
    else if (type == Type::DEBUG_BARY)
    {
        return "debug_bary";
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
    return "Material{ albedo=" + albedo.toString() + " + smooth_shading=" + Utils::stringFromBool(smoothShading) + " + type=" + StringFromType(type) + " }";

}

Vec3 Material::getAlbedo(const Scene& scene, const TraceHit& hit) const
{
    if (hasTexture) {
        const Texture& texture = scene.textures.at(textureIdx);
        return texture.getAlbedo(hit);
    }
    else {
        return albedo;
    }
}


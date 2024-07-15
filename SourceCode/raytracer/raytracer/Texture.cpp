#include "Texture.h"

#include <stdexcept>
#include <string>

TextureType Texture::TypeFromString(const std::string& type)
{
    if (type == "solid_color")
    {
        return TextureType::SOLID_COLOR;
    }
    else if (type == "edges")
    {
        return TextureType::EDGES;
    }
    else if (type == "checker")
    {
        return TextureType::CHECKER;
    }
    else if (type == "bitmap")
    {
        return TextureType::BITMAP;
    }
    else
    {
        throw std::runtime_error("Unknown texture type: " + type);
    }
}

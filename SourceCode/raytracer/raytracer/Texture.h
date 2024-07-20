#pragma once
#include <string>
#include "CRTTypes.h"

enum class TextureType {
    SOLID_COLOR,
    EDGES,
    CHECKER,
    BITMAP,
};


class TraceHit;
class Scene;
class Texture {
public:
    Texture(std::string& name, TextureType type) : name(name), type(type) {}
    TextureType type = TextureType::SOLID_COLOR;
    std::string name = "defaultTexture";
    Vec3 color1 {0.f, 0.f, 0.f}; // edge_color, color_A, albedo
    Vec3 color2 {0.f, 0.f, 0.f}; // inner_color, color_B
    float textureSize = 1.f; // edge_width, square_size
    std::string filePath = ""; // only BITMAP
    size_t bitmapIdx = 0; // only BITMAP

    static TextureType TypeFromString(const std::string& type);
    Vec3 getAlbedo(const Scene& scene, const TraceHit& hit) const;
};

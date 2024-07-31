#include "Camera.h"
#include "json.hpp"


/* warning: does not preserve roll. Will fix this with quaternions */

nlohmann::ordered_json Camera::toJson() const
{
    nlohmann::ordered_json j = SceneObject::toJson();
    j["fov"] = fov;
    return j;
}

Ray Camera::rayFromPixel(const Image& image, size_t x, size_t y) const {
    Vec3 coords{ static_cast<float>(x), static_cast<float>(y), 0 };
    ndcFromRaster(image, coords);
    imageFromNdc(image, coords);

    //Apply FOV to coordinates
    float fovRad = (fov * PI) / 180.0f; // Convert FOV to radians
    float tanHalfFov = std::tan(fovRad / 2.0f);
    coords.x *= tanHalfFov;
    coords.y *= tanHalfFov;

    Vec3 raydir = getDir() + getRight() * coords.x + getUp() * coords.y;
    raydir.normalize();
    return { this->pos, raydir };
}


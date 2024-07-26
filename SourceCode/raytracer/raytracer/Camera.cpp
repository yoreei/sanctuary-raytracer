#include "Camera.h"
#include "json.hpp"

nlohmann::ordered_json Camera::toJson() const
{
    nlohmann::ordered_json j = SceneObject::toJson();
    j["fov"] = fov;
    return j;
}


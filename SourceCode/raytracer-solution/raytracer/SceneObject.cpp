#include "include/SceneObject.h"
#include "json.hpp"

nlohmann::ordered_json SceneObject::toJson() const
{
    nlohmann::ordered_json j;
    j["pos"] = pos.toJson();
    j["mat"] = mat.toJson();
    return j;
}

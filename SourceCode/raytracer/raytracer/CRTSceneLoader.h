#pragma once
#include <string>
#include <vector>

#include "json.hpp"

class Triangle;
class Image;
class Scene;
class Vec3;

using json = nlohmann::json;

class CRTSceneLoader
{
public:
    static [[nodiscard]] bool loadCrtscene(const std::string& filename, Scene& scene, Image& image);
private:
    static bool parseLight(const json& j, Scene& scene);
    static bool parseBackgroundColor(const json& j, Scene& scene);
    static bool parseImageSettings(const json& j, Image& image);
    static bool parseCameraSettings(const json& j, Scene& scene);
    static bool parseObjects(const json& j, Scene& scene);
    static bool parseVertices(const json& jObj, Scene& scene);
    static bool parseTriangles(const json& jObj, Scene& scene);
};


#pragma once
#include <string>
#include <vector>
#include <memory>

#include "json.hpp"

class Settings;
class Triangle;
class Image;
class Scene;
class Vec3;

using json = nlohmann::json;

class CRTSceneLoader
{
public:
    static [[nodiscard]] bool loadCrtscene(const Settings& settings, const std::string& filename, Scene& scene, Image& image);
    static bool validateCrtscene(const json& j);
private:
    static bool parseLight(const json& j, Scene& scene);
    static bool parseBackgroundColor(const json& j, Scene& scene);
    static bool parseImageSettings(const json& j, Image& image);
    static bool parseCameraSettings(const json& j, Scene& scene);
    static bool parseTextures(const json& j, Scene& scene, const Settings& settings, std::map<std::string, size_t>& idxFromTextureName);
    static bool parseMaterials(const json& j, Scene& scene, const std::map<std::string, size_t>& idxFromTextureName);
    static std::shared_ptr<Animation> parseAnimation(const json& j, size_t idx);
    static Vec3 Vec3FromJson(const json& j);
    static bool boolFromJson(const json& j);
    static bool parseObjects(const json& j, Scene& scene);
    static bool parseVertices(const json& jObj, Scene& scene);
    static bool parseTriangles(const json& jObj, Scene& scene);
    static void genVertexNormals(Scene& scene);
    static void genAttachedTriangles(const Scene& scene, size_t vertexIndex, std::vector<size_t>& attachedTriangles);
    static bool getBoolDefault(const json& j, const std::string& key, bool defaultVal = false);
    template <typename T>
    static T getDefault(const json& j, const std::string& key, T defaultVal);
    static void warnIfMissing(const json& j, const std::string& key);
    template <typename T>
    static void assignIfExists(const json& j, std::string key, T& out);
    template <>
    void assignIfExists<Vec3>(const json& j, std::string key, Vec3& out);
    static void loadJpgBitmap(std::string filePath, Image& bitmap);
};


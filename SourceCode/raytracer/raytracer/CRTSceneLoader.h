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
    static bool parseImageSettings(const json& j, Image& image, const Settings& settings);
    static bool parseCameraSettings(const json& j, Scene& scene);
    static bool parseTextures(const json& j, Scene& scene, const Settings& settings, std::map<std::string, size_t>& idxFromTextureName);
    static bool parseMaterials(const json& j, Scene& scene, const std::map<std::string, size_t>& idxFromTextureName);
    static Vec3 Vec3FromJson(const json& j);
    static bool boolFromJson(const json& j);
    static bool parseObjects(const json& j, Scene& scene);
    static bool parseVertices(const json& jObj, std::vector<Vec3>& vertices);
    static void parseUvs(const json& jObj, const size_t expectedSize, std::vector<Vec3>& uvs);
    static bool parseTriangles(const json& jObj, const std::vector<Vec3>& vertices, const size_t materialIdx, std::vector<Triangle>& triangles);
    /*
    * populates `vertexNormals` with the average normal of all attached triangles. Important for smooth shading
    */
    static void calculateVertexNormals(const std::vector<Vec3>& vertices, const std::vector<Triangle>& triangles, std::vector<Vec3>& vertexNormals);
    /**
     * @brief Generate a list of triangle indexes that are attached to a vertex
     * 
     * @param scene Scene object
     * @param vertexIndex Index of the vertex
     * @param attachedTriangles Output list of triangle indexes
     */
    static void genAttachedTriangles(const size_t vertexIndex, const std::vector<Triangle>& triangles, std::vector<size_t>& attachedTriangles);
    static bool getBoolDefault(const json& j, const std::string& key, bool defaultVal = false);
    template <typename T>
    static T getDefault(const json& j, const std::string& key, T defaultVal);
    static void warnIfMissing(const json& j, const std::string& key);
    template <typename T>
    static void assignIfExists(const json& j, std::string key, T& out);
    template <>
    void assignIfExists<Vec3>(const json& j, std::string key, Vec3& out);
    static void loadBitmap(std::string filePath, Image& bitmap);
    static void debugPrintNormals(const Scene& scene);
};


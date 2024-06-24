#pragma once

#include <fstream>

#include "CRTTypes.h"
#include "Camera.h"
#include "Settings.h"
#include "Triangle.h"
#include "Animation.h"
#include "MeshObject.h"
#include "Image.h"

#include "json.hpp"

using json = nlohmann::json;

class Scene
{
public:
    Camera camera {};
    Settings settings {};

    /* meshObjects reference trinagles. Triangles reference vertices */
    std::vector<Vec3> vertices {};
    std::vector<Triangle> triangles {};
    std::vector<MeshObject> meshObjects {};
    
    Color bgColor = Color{0, 0, 0};

    [[nodiscard]] bool loadCrtscene(const std::string& filename, Image& image){
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Unable to open file: " << filename << '\n';
            return false;
        }

        json j;
        try {
            file >> j;
        } catch (const json::exception& e) {
            std::cerr << "Error parsing JSON file: " << e.what() << '\n';
            return false;
        }

        if (!parseBackgroundColor(j) ||
            !parseImageSettings(j, image) ||
            !parseCameraSettings(j) ||
            !parseObjects(j)) {
            return false;
        }

        return true;
    }

    /* Compiles all vertices and triangles into a single mesh object */
    [[nodiscard]] bool bakeObject() {
        for (const auto& tri : triangles) {
            if (tri.v[0] >= vertices.size() || tri.v[1] >= vertices.size() || tri.v[2] >= vertices.size()) {
                std::cerr << "Error loading triangles: vertex index out of bounds\n";
                return false;
            }
        }

        meshObjects.emplace_back(0, triangles.size() - 1);

        return true;
    }

    void addObjects(const std::vector<Scene>& scenes)
    {
        for (const auto& scene : scenes) {
            size_t verticesPadding = vertices.size();
            vertices.reserve(vertices.size() + scene.vertices.size());
            vertices.insert(vertices.end(), scene.vertices.begin(), scene.vertices.end());

            size_t trianglesPadding = triangles.size();
            triangles.reserve(triangles.size() + scene.triangles.size());
            triangles.insert(triangles.end(), scene.triangles.begin(), scene.triangles.end());

            for (size_t i = trianglesPadding; i < triangles.size(); i++) {
                triangles[i].v[0] += verticesPadding;
                triangles[i].v[1] += verticesPadding;
                triangles[i].v[2] += verticesPadding;
            }

            size_t meshObjectsPadding = meshObjects.size();
            meshObjects.reserve(meshObjects.size() + scene.meshObjects.size());
            meshObjects.insert(meshObjects.end(), scene.meshObjects.begin(), scene.meshObjects.end());

            for (size_t i = meshObjectsPadding; i < meshObjects.size(); ++i) {
                for (size_t j = 0; j < meshObjects[i].triangles.size(); ++j) {
                    meshObjects[i].triangles[j] += trianglesPadding;
                }
            }

        }
    }

private:

    bool parseBackgroundColor(const json& j) {
        try {
            const auto& jBgColor = j.at("settings").at("background_color");

            if (!jBgColor.is_array() || jBgColor.size() != 3) {
                std::cerr << "Error loading background_color: background_color is not an array or not of size 3\n";
                return false;
            }

            this->bgColor = Color::fromUnit(jBgColor[0], jBgColor[1], jBgColor[2]);
            return true;
        }
        catch (const json::exception& e) {
            std::cerr << "Error loading background_color: " << e.what() << '\n';
            return false;
        }
    }

    bool parseImageSettings(const json& j, Image& image) {
        try {
            const auto& jImgSettings = j.at("settings").at("image_settings");
            image = Image(jImgSettings.at("width"), jImgSettings.at("height"));
            return true;
        }
        catch (const json::exception& e) {
            std::cerr << "Error loading image_settings: " << e.what() << '\n';
            return false;
        }
    }

    bool parseCameraSettings(const json& j) {
        try {
            const auto& jCamPos = j.at("camera").at("position");
            Vec3 camPos{ jCamPos.at(0), jCamPos.at(1), jCamPos.at(2) };

            const auto& jCamMat = j.at("camera").at("matrix");
            Matrix3x3 camMat{ jCamMat.get<std::vector<float>>() };

            this->camera = Camera{ 90.f, camPos, camMat };
            return true;
        }
        catch (const json::exception& e) {
            std::cerr << "Error loading camera settings: " << e.what() << '\n';
            return false;
        }
    }

    bool parseObjects(const json& j) {
        std::vector<Scene> scenes {};

        try {
            const auto& jObjects = j.at("objects");
            for (const auto& jObj : jObjects) {
                Scene scene {};

                if (!Scene::parseVertices(jObj, scene.vertices)) {
                    return false;
                }
                if (!Scene::parseTriangles(jObj, scene.triangles)) {
                    return false;
                }

                if(!scene.bakeObject()) {
                    return false;
                }

                scenes.push_back(scene);
            }

        }
        catch (const json::exception& e) {
            std::cerr << "Error loading objects: " << e.what() << '\n';
            return false;
        }
        addObjects(scenes);
        return true;
    }

    static bool parseVertices(const json& jObj, std::vector<Vec3>& vertices) {
        try {
            const auto& jVertices = jObj.at("vertices");

            if (!jVertices.is_array() || jVertices.size() % 3 != 0) {
                std::cerr << "Error loading vertices: vertices is not an array or not divisible by 3\n";
                return false;
            }

            for (size_t i = 0; i < jVertices.size(); i += 3) {
                vertices.emplace_back(jVertices[i], jVertices[i + 1], jVertices[i + 2]);
            }
        }
        catch (const json::exception& e) {
            std::cerr << "Error loading vertices: " << e.what() << '\n';
            return false;
        }

        return true;
    }

    static bool parseTriangles(const json& jObj, std::vector<Triangle>& triangles) {
        try {
            const auto& jTriangles = jObj.at("triangles");

            if (!jTriangles.is_array() || jTriangles.size() % 3 != 0) {
                std::cerr << "Error loading triangles: triangles is not an array or not divisible by 3\n";
                return false;
            }

            for (size_t i = 0; i < jTriangles.size(); i += 3) {
                triangles.emplace_back(jTriangles[i], jTriangles[i + 1], jTriangles[i + 2]);
            }
        }
        catch (const json::exception& e) {
            std::cerr << "Error loading triangles: " << e.what() << '\n';
            return false;
        }

        return true;
    }
};

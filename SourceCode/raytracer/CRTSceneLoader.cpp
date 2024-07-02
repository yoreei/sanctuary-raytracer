#include <fstream>

#include "json.hpp"

#include "CRTSceneLoader.h"
#include "Scene.h"
#include "Image.h"

using json = nlohmann::json;

[[nodiscard]]
bool CRTSceneLoader::loadCrtscene(const std::string& filename, Scene& scene, Image& image) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Unable to open file: " << filename << '\n';
        return false;
    }

    json j;
    try {
        file >> j;
    }
    catch (const json::exception& e) {
        std::cerr << "Error parsing JSON file: " << e.what() << '\n';
        return false;
    }

    if (!parseBackgroundColor(j, scene) ||
        !parseImageSettings(j, image) ||
        !parseCameraSettings(j, scene) ||
        !parseObjects(j, scene) ||
        !parseLight(j, scene)) {
        return false;
    }
    return true;
}

bool CRTSceneLoader::parseLight(const json& j, Scene& scene)
{
    if (!j.contains("lights") || j.at("lights").empty()) {
        std::cerr << "Heads up: no lights found!\n";
        return true;
    }
    else if (!j.at("lights").is_array()) {
        std::cerr << "Error loading lights: lights is not an array\n";
        return false;
    }
    else {
        for (const auto& jLight : j.at("lights")) {
            if (!jLight.contains("intensity") || !jLight.contains("position")) {
                std::cerr << "Error loading light: intensity or position not found\n";
                return false;
            }

            if (!jLight.at("position").is_array() || jLight.at("position").size() != 3) {
                std::cerr << "Error loading light: position is not an array or not of size 3\n";
                return false;
            }

            float intensity = jLight.at("intensity");
            Vec3 pos{ jLight.at("position")[0], jLight.at("position")[1], jLight.at("position")[2] };
            scene.lights.emplace_back(pos, intensity, Color{0, 0, 0});
        }
    }
    return true;
}

inline bool CRTSceneLoader::parseBackgroundColor(const json& j, Scene& scene) {
    try {
        const auto& jBgColor = j.at("settings").at("background_color");

        if (!jBgColor.is_array() || jBgColor.size() != 3) {
            std::cerr << "Error loading background_color: background_color is not an array or not of size 3\n";
            return false;
        }

        scene.bgColor = Color::fromUnit(jBgColor[0], jBgColor[1], jBgColor[2]);
        return true;
    }
    catch (const json::exception& e) {
        std::cerr << "Error loading background_color: " << e.what() << '\n';
        return false;
    }
}

inline bool CRTSceneLoader::parseImageSettings(const json& j, Image& image) {
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

inline bool CRTSceneLoader::parseCameraSettings(const json& j, Scene& scene) {
    try {
        const auto& jCamPos = j.at("camera").at("position");
        Vec3 camPos{ jCamPos.at(0), jCamPos.at(1), jCamPos.at(2) };

        const auto& jRotateMat = j.at("camera").at("matrix");
        Matrix3x3 rotateMat{ jRotateMat.get<std::vector<float>>() };
        Matrix3x3 camMat = rotateMat * Camera::DefaultMatrix;
        //Matrix3x3 camMat = rotateMat;
        

        scene.camera = Camera{ 90.f, camPos, camMat };
        return true;
    }
    catch (const json::exception& e) {
        std::cerr << "Error loading camera settings: " << e.what() << '\n';
        return false;
    }
}

inline bool CRTSceneLoader::parseObjects(const json& j, Scene& scene) {
    std::vector<Scene> scenes {};

    try {
        const auto& jObjects = j.at("objects");
        for (const auto& jObj : jObjects) {
            Scene scene{};

            if (!parseVertices(jObj, scene)) {
                return false;
            }
            if (!parseTriangles(jObj, scene)) {
                return false;
            }

            if (!scene.bakeObject()) {
                return false;
            }

            scenes.push_back(scene);
        }

    }
    catch (const json::exception& e) {
        std::cerr << "Error loading objects: " << e.what() << '\n';
        return false;
    }
    scene.addObjects(scenes);
    return true;
}

inline bool CRTSceneLoader::parseVertices(const json& jObj, Scene& scene) {
    auto& vertices = scene.vertices;
    try {
        const auto& jVertices = jObj.at("vertices");

        if (!jVertices.is_array() || jVertices.size() % 3 != 0) {
            std::cerr << "Error loading vertices: vertices is not an array or not divisible by 3\n";
            return false;
        }

        for (size_t i = 0; i < jVertices.size(); i += 3) {
            float y = static_cast<float>(jVertices[i + 1]);
            vertices.emplace_back(jVertices[i], y, jVertices[i + 2]);
        }
    }
    catch (const json::exception& e) {
        std::cerr << "Error loading vertices: " << e.what() << '\n';
        return false;
    }

    return true;
}

inline bool CRTSceneLoader::parseTriangles(const json& jObj, Scene& scene) {
    auto& triangles = scene.triangles;
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

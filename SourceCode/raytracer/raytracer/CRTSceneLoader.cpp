#pragma warning( disable : 4365 )

#include <fstream>

#include <memory>
#include "json.hpp"

#include "Animation.h"
#include "CRTSceneLoader.h"
#include "Scene.h"
#include "Image.h"
#include "Material.h"
#include "Texture.h"

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

    std::cout << "Loading Scene: " << filename << std::endl;

    if (!validateCrtscene(j)) {
        return false;
    }

    if (!parseBackgroundColor(j, scene) ||
        !parseImageSettings(j, image) ||
        !parseCameraSettings(j, scene) ||
        !parseTextures(j, scene) ||
        !parseMaterials(j, scene) ||
        !parseObjects(j, scene) ||
        !parseLight(j, scene)) {
        return false;
    }

    scene.generateVertexNormals();
    return true;
}

bool CRTSceneLoader::validateCrtscene(const json& j) {
    // TODO: move more validation to this function
    for (const auto& element : j.items()) {
        const std::string& key = element.key();
        const auto& val = element.value();
        if (key == "settings") {
            if (!val.contains("background_color") || !val.contains("image_settings")) {
                std::cerr << "Error loading settings: background_color or image_settings not found\n";
                return false;
            }
        }
        else if (key == "camera") {
            if (!val.contains("position") || !val.contains("matrix")) {
                std::cerr << "Error loading camera: position or matrix not found\n";
                return false;
            }
        }
        else if (key == "materials") {
            if (!val.is_array() && val.size() > 0) {
                std::cerr << "Error loading materials: materials is not an array\n";
                return false;
            }
            for (size_t i = 0; i < val.size(); ++i) {
                const auto& jMaterial = val[i];
                if (!jMaterial.contains("type")) {
                    std::cerr << "Error loading material[" << i << "]: type not found\n";
                    return false;
                }
            }
        }
        else if (key == "objects") {
            if (!val.is_array() && val.size() > 0) {
                std::cerr << "Error loading objects: objects is not an array\n";
                return false;
            }
            for (const auto& jObj : val) {
                if (!jObj.contains("vertices") || !jObj.contains("triangles")) {
                    std::cerr << "Error loading object: vertices or triangles or material_index not found\n";
                    return false;
                }

                const auto& jTriangles = jObj.at("triangles");
                if (!jTriangles.is_array() || jTriangles.size() % 3 != 0) {
                    std::cerr << "Error loading triangles: triangles is not an array or not divisible by 3\n";
                    return false;
                }
            }
        }
        else if (key == "lights") {
            if (!val.is_array()) {
                std::cerr << "Error loading lights: lights is not an array\n";
                return false;
            }
        }
        else if (key == "animations") {
            //	"animations": [{
            //	"type": "truck",
            //	"start_frame": 0,
            //	"end_frame": 10,
            //	"delta": 5
            //}],
            if (!val.is_array()) {
                std::cerr << "Error loading animations: animations is not an array\n";
                return false;
            }
            for (const auto& jAnim : val) {
                if (!jAnim.contains("type") ||
                    !jAnim.contains("start_frame") ||
                    !jAnim.contains("end_frame")
                    ) {
                    std::cerr << "Error loading animation: type or start_frame or end_frame not found\n";
                    return false;
                }
            }
        }
        else {
            std::cerr << "Error loading CRTScene: unknown key: " << key << '\n';
            return false;
        }
    }
    return true;
}

bool CRTSceneLoader::parseLight(const json& j, Scene& scene)
{
    warnIfMissing(j, "lights");
    if (!j.contains("lights") || j.at("lights").empty()) {
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
            Light light{ pos, intensity };
            scene.lights.push_back(light);
        }
    }
    return true;
}

std::shared_ptr<Animation> CRTSceneLoader::parseAnimation(const json& j, size_t idx) {
    assert(j.is_object());
    assert(j.contains("animations"));
    const auto& jAnims = j.at("animations");
    const auto& jAnim = jAnims[idx];
    int startFrame = jAnim.at("start_frame");
    int endFrame = jAnim.at("end_frame");
    std::string type = jAnim.at("type");
    if (type == "truck") {
        float delta = jAnim.at("delta");
        auto truckAnim = MoveAnimation::Make(MoveType::Truck, delta, startFrame, endFrame);
        return truckAnim;
    }
    else {
        std::cerr << "Error loading animation: unknown type: " << type << '\n';
        return nullptr;
    }
}

bool CRTSceneLoader::parseBackgroundColor(const json& j, Scene& scene) {
    try {
        const auto& jBgColor = j.at("settings").at("background_color");

        if (!jBgColor.is_array() || jBgColor.size() != 3) {
            std::cerr << "Error loading background_color: background_color is not an array or not of size 3\n";
            return false;
        }

        scene.bgColor = Vec3{ jBgColor[0], jBgColor[1], jBgColor[2] };
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
    const auto& jCam = j.at("camera");
    const auto& jCamPos = jCam.at("position");
    Vec3 camPos{ jCamPos.at(0), jCamPos.at(1), jCamPos.at(2) };

    const auto& jRotateMat = jCam.at("matrix");
    Matrix3x3 rotateMat{ jRotateMat.get<std::vector<float>>() };
    // NB: CRTScene format gives us the inverse direction of the camera.
    Matrix3x3 camMat = Camera::DefaultMatrix * rotateMat;

    scene.camera = Camera{ 90.f, camPos, camMat };

    if (jCam.contains("animation_indexes")) {
        const auto& animation_indexes = jCam.at("animation_indexes");
        for (size_t i = 0; i < animation_indexes.size(); ++i) {
            int animationIdx = animation_indexes[i];
            auto anim = parseAnimation(j, animationIdx);
            scene.animator.addAnimation(scene.camera, anim);
        }
    }

    return true;
}

inline bool CRTSceneLoader::parseTextures(const json& j, Scene& scene) {
    if (!j.contains("textures")) {
        std::cerr << "No textures found\n";
        return true;
    }

    for (const auto& jTexture : j.at("textures")) {
        std::string name = jTexture.at("name");
        std::string typeStr = jTexture.at("type");
        TextureType type = Texture::TypeFromString(typeStr);
        Texture tex{ name, type };

        assignIfExists<Vec3>(jTexture, "albedo", tex.color1);
        assignIfExists<Vec3>(jTexture, "edge_color", tex.color1);
        assignIfExists<Vec3>(jTexture, "color_A", tex.color1);
        assignIfExists<Vec3>(jTexture, "inner_color", tex.color2);
        assignIfExists<Vec3>(jTexture, "color_B", tex.color2);
        assignIfExists<float>(jTexture, "edge_width", tex.textureSize);
        assignIfExists<float>(jTexture, "square_size", tex.textureSize);
        assignIfExists<std::string>(jTexture, "file_path", tex.filePath);

        scene.textures.push_back(tex);
    }
    return true;
}

inline bool CRTSceneLoader::parseMaterials(const json& j, Scene& scene) {
    warnIfMissing(j, "materials");
    if (!j.contains("materials")) {
        scene.materials.push_back(Material{});
        std::cerr << "Substituting default material\n";
        return true;
    }

    for (auto& jMaterial : j.at("materials")) {
        scene.materials.emplace_back();
        Material& material = scene.materials.back();
        material.smoothShading = getDefault<bool>(jMaterial, "smooth_shading", false);
        material.type = Material::TypeFromString(jMaterial.at("type"));
        material.occludes = getDefault<bool>(jMaterial, "occludes", true);
        material.ior = getDefault<float>(jMaterial, "ior", 1.f);
        material.albedo = Vec3{ 1.f, 1.f, 1.f };
        if (jMaterial.contains("albedo")) {
            material.albedo = Vec3FromJson(jMaterial.at("albedo"));
        }
        if (material.type == Material::Type::REFRACTIVE) {
            material.albedo = Vec3{ 1.f, 1.f, 1.f };
            material.occludes = false;
            material.transparency = 1.f;
            material.reflectivity = 1.f;
        }

        else if (material.type == Material::Type::REFLECTIVE) {
            material.albedo = Vec3{ 1.f, 1.f, 1.f };
            material.reflectivity = .8f;
        }
    }

    return true;

    /*
        "materials": [{
                "type": "diffuse",
                "albedo": [
                    0, 1, 1
                ],
                "smooth_shading": false
            }
        ],
    */

}

inline Vec3 CRTSceneLoader::Vec3FromJson(const json& j) {
    return Vec3{ j[0], j[1], j[2] };
}

inline bool CRTSceneLoader::boolFromJson(const json& j) {
    if (!j.is_boolean()) {
        std::cerr << "Error loading bool: not a boolean\n";
        return false;
    }
    return j.get<bool>();
}

inline bool CRTSceneLoader::parseObjects(const json& j, Scene& scene) {
    std::vector<Scene> scenes {};

    try {
        const auto& jObjects = j.at("objects");
        for (const auto& jObj : jObjects) {
            Scene objScene{ "tempScene" };

            if (!parseVertices(jObj, objScene)) {
                return false;
            }
            if (!parseTriangles(jObj, objScene)) {
                return false;
            }

            if (!objScene.bakeObject()) {
                return false;
            }

            scenes.push_back(objScene);
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
    warnIfMissing(jObj, "material_index");
    size_t materialIndex = getDefault<size_t>(jObj, "material_index", 0); // 0 is the default material index. It is guaranteed to exist

    const auto& jTriangles = jObj.at("triangles");
    auto& triangles = scene.triangles;
    for (size_t i = 0; i < jTriangles.size(); i += 3) {
        triangles.emplace_back(scene.vertices, jTriangles[i], jTriangles[i + 1], jTriangles[i + 2], materialIndex);
    }

    return true;
}

template <typename T>
T CRTSceneLoader::getDefault(const json& j, const std::string& key, T defaultVal) {
    if (!j.contains(key)) {
        return defaultVal;
    }
    return j.at(key).get<T>();
}

template <>
Vec3 CRTSceneLoader::getDefault<Vec3>(const json& j, const std::string& key, Vec3 defaultVal) {
    if (!j.contains(key)) {
        return defaultVal;
    }

    return Vec3FromJson(j.at(key));
}

void CRTSceneLoader::warnIfMissing(const json& j, const std::string& key) {
    if (!j.contains(key)) {
        std::cerr << "CRTSceneLoader::warnIfMissing: key " << key << " not found\n";
    }
}


template <typename T>
void CRTSceneLoader::assignIfExists(const json& j, std::string key, T& out) {
    if (j.contains(key)) {
        out = j.at(key).get<T>();
    }
}

template <>
void CRTSceneLoader::assignIfExists<Vec3>(const json& j, std::string key, Vec3& out) {
    if (j.contains(key)) {
        out = Vec3FromJson(j.at(key));
    }
}




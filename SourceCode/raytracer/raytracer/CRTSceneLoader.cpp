#pragma warning( disable : 4365 )

#include <fstream>

#include <memory>
#include "json.hpp"
#include "stb_image.h"

#include "Animation.h"
#include "CRTSceneLoader.h"
#include "Scene.h"
#include "Image.h"
#include "Material.h"
#include "Texture.h"
#include "Settings.h"

using json = nlohmann::json;

[[nodiscard]]
bool CRTSceneLoader::loadCrtscene(const Settings& settings, const std::string& filename, Scene& scene, Image& image) {
    const std::string filePath = settings.inputDir + "/" + filename;
    std::ifstream file(filePath);
    if (!file) {
        throw std::runtime_error("Failed to load CRTScene file: " + filePath);
        return false;
    }

    json j;
    file >> j;

    std::cout << "Loading Scene: " << filename << std::endl;

    if (!validateCrtscene(j)) {
        return false;
    }

    if (!parseBackgroundColor(j, scene) ||
        !parseImageSettings(j, image) ||
        !parseCameraSettings(j, scene) ||
        !parseTextures(j, scene, settings) ||
        !parseMaterials(j, scene) ||
        !parseObjects(j, scene) ||
        !parseLight(j, scene)) {
        return false;
    }

    // TODO performance: perform genAttachedTriangles (in generateVertexNormals) during Scene loading
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
        else if (key == "textures") {}
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
    const auto& jImgSettings = j.at("settings").at("image_settings");
    size_t width = jImgSettings.at("width").get<size_t>();
    size_t height = jImgSettings.at("height").get<size_t>();
    image = Image(width, height);
    return true;
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

inline bool CRTSceneLoader::parseTextures(const json& j, Scene& scene, const Settings& settings)
{
    if (!j.contains("textures")) {
        std::cerr << "No textures found\n";
        return true;
    }

    auto& jTextures = j.at("textures");
    for (size_t i = 0; i < jTextures.size(); ++i) {
        const auto& jTexture = jTextures[i];
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
        tex.filePath = settings.inputDir + "/" + tex.filePath;

        if (type == TextureType::BITMAP) {
            Image bitmap;
            loadJpgBitmap(tex.filePath, bitmap);
            size_t bitmapIdx = scene.addBitmap(std::move(bitmap));
            tex.bitmapIdx = bitmapIdx;
        }

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
            if (jMaterial.at("albedo").is_string()) {
                std::string textureName = jMaterial.at("albedo").get<std::string>();
            }
            else if (jMaterial.at("albedo").is_array()) {
                material.albedo = Vec3FromJson(jMaterial.at("albedo"));
            }
            else { throw std::runtime_error("Error loading albedo: albedo is not a string or array\n"); }
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

            scenes.push_back(std::move(objScene));
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

struct TestPixel {
    unsigned char r, g, b;
};

struct TestImage {
    size_t width, height;
    std::vector<TestPixel> data;

    TestImage(size_t w, size_t h) : width(w), height(h), data(w * h) {}
};

void CRTSceneLoader::loadJpgBitmap(std::string filePath, Image& bitmap)
{
    std::ifstream file(filePath);
    if (!file) {
        throw std::runtime_error("Failed to load image");
    }

    int width, height, channels;
    int ok = stbi_info(filePath.c_str(), &width, &height, &channels);
    if (ok != 1) {
        throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
    }
    Image img {size_t(width), size_t(height)};

    unsigned char* stbi_ptr = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    if (stbi_ptr == nullptr || channels < 3 || channels > 4) {
        throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
    }

    for (size_t i = 0; i < width * height; ++i)
    {
        img.data[i].r = stbi_ptr[i * channels];
        img.data[i].g = stbi_ptr[i * channels + 1];
        img.data[i].b = stbi_ptr[i * channels + 2];
    }

    stbi_image_free(stbi_ptr);

    bitmap = std::move(img); 
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

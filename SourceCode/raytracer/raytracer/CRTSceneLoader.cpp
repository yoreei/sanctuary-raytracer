#pragma warning( disable : 4365 )

#include <fstream>

#include <memory>
#include "json.hpp"
#include "stb_image.h"

#include "CRTSceneLoader.h"
#include "Scene.h"
#include "Image.h"
#include "Material.h"
#include "Texture.h"
#include "Settings.h"
#include "Utils.h"

using json = nlohmann::json;

[[nodiscard]]
bool CRTSceneLoader::loadCrtscene(const Settings& settings, const std::string& filePath, Scene& scene, Image& image) {
    // Ensure Scene is clean:
    scene = Scene{scene.fileName, &settings};

    std::ifstream file(filePath);
    if (!file) {
        throw std::runtime_error("Failed to load CRTScene file: " + filePath);
        return false;
    }

    json j;
    file >> j;

    std::cout << "Loading Scene: " << filePath << std::endl;

    if (!validateCrtscene(j)) {
        return false;
    }

    std::map<std::string, size_t> idxFromTextureName;
    if (!parseBackgroundColor(j, scene) ||
        !parseImageSettings(j, image, settings) ||
        !parseCameraSettings(j, scene) ||
        !parseTextures(j, scene, settings, idxFromTextureName) ||
        !parseMaterials(j, scene, idxFromTextureName) ||
        !parseObjects(j, scene) ||
        !parseLight(j, scene)) {
        return false;
    }

    loadBitmap(settings.sceneLibraryDir + "/skybox/industrial_sunset_puresky/0001.png", scene.cubemap.images[0]);
    loadBitmap(settings.sceneLibraryDir + "/skybox/industrial_sunset_puresky/0002.png", scene.cubemap.images[1]);
    loadBitmap(settings.sceneLibraryDir + "/skybox/industrial_sunset_puresky/0003.png", scene.cubemap.images[2]);
    loadBitmap(settings.sceneLibraryDir + "/skybox/industrial_sunset_puresky/0004.png", scene.cubemap.images[3]);
    loadBitmap(settings.sceneLibraryDir + "/skybox/industrial_sunset_puresky/0005.png", scene.cubemap.images[4]);
    loadBitmap(settings.sceneLibraryDir + "/skybox/industrial_sunset_puresky/0006.png", scene.cubemap.images[5]);

    scene.build();

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

inline bool CRTSceneLoader::parseImageSettings(const json& j, Image& image, const Settings& settings) {
    const auto& jImgSettings = j.at("settings").at("image_settings");

    if (settings.overrideResolution) {
        image = Image{ settings.resolutionX, settings.resolutionY };
    }
    else {
        size_t width = jImgSettings.at("width").get<size_t>();
        size_t height = jImgSettings.at("height").get<size_t>();

        image = Image{ width, height };
    }

    if (settings.debugPixel) {
        // Narrow down the pixel to debug
        image.startPixelY = settings.debugPixelY;
        image.endPixelY = settings.debugPixelY + 1;
        image.startPixelX = settings.debugPixelX;
        image.endPixelX = settings.debugPixelX + 1;
    }

    if (jImgSettings.contains("bucket_size")) {
        image.bucketSize = jImgSettings.at("bucket_size").get<size_t>();
    }

    if (settings.forceSingleThreaded || settings.debugPixel) {
        image.bucketSize = image.getWidth() * image.getHeight();
    }

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

    return true;
}

inline bool CRTSceneLoader::parseTextures(const json& j, Scene& scene, const Settings& settings, std::map<std::string, size_t>& idxFromTextureName)
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
        tex.filePath = settings.sceneLibraryDir + "/" + settings.projectDir + "/" + tex.filePath;

        if (type == TextureType::BITMAP) {
            loadBitmap(tex.filePath, tex.bitmap);
        }

        idxFromTextureName[name] = i;
        scene.textures.push_back(std::move(tex));
    }
    return true;
}

inline bool CRTSceneLoader::parseMaterials(const json& j, Scene& scene, const std::map<std::string, size_t>& idxFromTextureName) {
    warnIfMissing(j, "materials");
    if (!j.contains("materials")) {
        scene.materials.push_back(Material{});
        std::cerr << "Substituting default material\n";
        return true;
    }

    for (auto& jMaterial : j.at("materials")) {
        scene.materials.emplace_back();
        Material& material = scene.materials.back();
        material.smoothShading = Utils::jsonGetDefault<bool>(jMaterial, "smooth_shading", false);
        material.type = Material::TypeFromString(jMaterial.at("type"));
        material.occludes = Utils::jsonGetDefault<bool>(jMaterial, "occludes", true);
        material.ior = Utils::jsonGetDefault<float>(jMaterial, "ior", 1.f);
        material.setAlbedo({ 1.f, 1.f, 1.f });
        if (jMaterial.contains("albedo")) {
            if (jMaterial.at("albedo").is_string()) {
                std::string textureName = jMaterial.at("albedo").get<std::string>();
                material.textureIdx = idxFromTextureName.at(textureName);
                material.hasTexture = true;
            }
            else if (jMaterial.at("albedo").is_array()) {
                material.setAlbedo(Vec3FromJson(jMaterial.at("albedo")));
                material.hasTexture = false;
            }
            else { throw std::runtime_error("Error loading albedo: albedo is not a string or array\n"); }
        }
        if (material.type == Material::Type::REFRACTIVE) {
            material.occludes = false;
            material.transparency = 1.f;
            material.reflectivity = 1.f;
        }

        else if (material.type == Material::Type::REFLECTIVE) {
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
    const auto& jObjects = j.at("objects");
    for (const auto& jObj : jObjects) {
        std::vector<Vec3> vertices;
        std::vector<Triangle> triangles;
        std::vector<Vec3> uvs;
        std::vector<Vec3> vertexNormals;

        parseVertices(jObj, vertices);

        warnIfMissing(jObj, "material_index");
        size_t materialIdx = Utils::jsonGetDefault<size_t>(jObj, "material_index", 0);
        // 0 is the default material index. It is guaranteed to exist
        assert(scene.materials.size() > materialIdx);
        parseTriangles(jObj, vertices, materialIdx, triangles);

        parseUvs(jObj, vertices.size(), uvs);
        calculateVertexNormals(vertices, triangles, vertexNormals);

        assert(triangles.size() > 0);
        scene.addObject(vertices, triangles, vertexNormals, uvs);
    }

    return true;
}

void CRTSceneLoader::calculateVertexNormals(const std::vector<Vec3>& vertices, const std::vector<Triangle>& triangles, std::vector<Vec3>& vertexNormals) {
    // Vec3{0.f, 0.f, 0.f} is important for summation
    vertexNormals.resize(vertices.size(), Vec3{ 0.f, 0.f, 0.f });
    for (size_t i = 0; i < vertices.size(); ++i) {
        std::vector<size_t> attachedTriangles {};
        genAttachedTriangles(i, triangles, attachedTriangles);
        for (size_t triIndex : attachedTriangles) {
            vertexNormals[i] += triangles[triIndex].getNormal();
        }
    }

    for (size_t i = 0; i < vertexNormals.size(); ++i) {
        auto& normal = vertexNormals[i];
        normal.normalize();
    }

}

void CRTSceneLoader::genAttachedTriangles(const size_t vertexIndex, const std::vector<Triangle>& triangles, std::vector<size_t>& attachedTriangles) {
    for (size_t i = 0; i < triangles.size(); ++i) {
        const Triangle& tri = triangles[i];
        if (tri.hasVertex(vertexIndex)) {
            attachedTriangles.push_back(i);
        }
    }
}

inline bool CRTSceneLoader::parseVertices(const json& jObj, std::vector<Vec3>& vertices) {
    const auto& jVertices = jObj.at("vertices");

    if (!jVertices.is_array() || jVertices.size() % 3 != 0) {
        throw std::runtime_error("Error loading vertices: vertices is not an array or not divisible by 3");
    }

    for (size_t i = 0; i < jVertices.size(); i += 3) {
        float x = jVertices[i].get<float>();
        float y = jVertices[i + 1].get<float>();
        float z = jVertices[i + 2].get<float>();
        vertices.emplace_back(x, y, z);
    }

    return true;
}

void CRTSceneLoader::parseUvs(const json& jObj, const size_t expectedSize, std::vector<Vec3>& uvs)
{
    if (jObj.contains("uvs")) {
        const auto& jUvs = jObj.at("uvs");

        for (size_t i = 0; i < jUvs.size(); i += 3) {
            float u = jUvs[i].get<float>();
            float v = jUvs[i + 1].get<float>();
            float w = jUvs[i + 2].get<float>();
            uvs.emplace_back(u, v, w);
        }
    }
    else {
        uvs.resize(expectedSize, Vec3{ 0.f, 0.f, 0.f });
    }

    assert(uvs.size() == expectedSize);
}

inline bool CRTSceneLoader::parseTriangles(const json& jObj, const std::vector<Vec3>& vertices, const size_t materialIdx, std::vector<Triangle>& triangles) {

    const auto& jTriangles = jObj.at("triangles");
    for (size_t i = 0; i < jTriangles.size(); i += 3) {
        triangles.emplace_back(vertices, jTriangles[i], jTriangles[i + 1], jTriangles[i + 2], materialIdx);
    }

    return true;
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

    TestImage(size_t w, size_t h) : width(w), height(h), data(w* h) {}
};

void CRTSceneLoader::loadBitmap(std::string filePath, Image& bitmap)
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
    Image img{ size_t(width), size_t(height) };

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

void CRTSceneLoader::debugPrintNormals(const Scene& scene)
{
    std::cout << "Vertex Normals:\n";
    for (size_t i = 0; i < scene.vertexNormals.size(); ++i) {
        const auto& vertNormal = scene.vertexNormals[i];
        std::cout << i << ": " << vertNormal << '\n';
    }

    std::cout << "Triangle Normals:\n";
    for (size_t i = 0; i < scene.triangles.size(); ++i) {
        const auto& tri = scene.triangles[i];
        std::cout << i << ": " << tri.getNormal() << '\n';
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

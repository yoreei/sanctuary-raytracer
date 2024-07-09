#pragma once
#include <algorithm>
#include <tuple>
#include <queue>

#include "Camera.h"
#include "CRTTypes.h"
#include "Image.h"
#include "Scene.h"
#include "Triangle.h"
#include "Tasks.h"
#include "Intersection.h"

#include "perlin-noise.h"

class Renderer {
public:
    size_t maxDepth = 3;
    Renderer() {}

    void renderScene(const Scene& scene, Image& image)
    {
        // Prepare Primary Queue
        scene.metrics.startTimer();
        // TODO reserve space for the queue
        Ray ray;
        for (int y = 0; y < image.getHeight(); ++y) {
            for (int x = 0; x < image.getWidth(); ++x) {
                scene.camera.emplaceTask(image, x, y, traceQueue);
            }
        }

        processTraceQueue(scene, image);
        //processShadowQueue(scene, image);
        scene.metrics.stopTimer();
    }

    /*
    * DEBUGGING
    * Fires a single ray.
    */
    void _DBGrenderScene(const Scene& scene, Image& image, Ray ray = {{0.f, 0.f, 0.f}, {-1.f, -1.f, -1.f}})
    {
        //ray.direction.normalize();
        ray.origin = { 0.f, 0.f, 0.f };
        ray.direction = { -0.310658336f, -0.208651125f, -0.927338243f };
        TraceTask task = { ray, 0, 0,};
        traceQueue.push(task);
        processTraceQueue(scene, image);
    }

    std::queue<TraceTask> traceQueue {};
    float shadowBias = 0.001f;
    float refractionBias = 0.001f;
private:

    void processTraceQueue(const Scene& scene, Image& image)
    {
        while (!traceQueue.empty()) {
            TraceTask task = traceQueue.front();
            Ray& ray = task.ray;
            traceQueue.pop();
            Intersection xData{};
            scene.intersect(ray, xData);
            processXData(scene, image, task, xData);
        }
    }

    void processXData(const Scene& scene, Image& image, TraceTask& task, Intersection& xData) {
        if (!xData.successful()) {
            shadeVoid(scene, image, task);
            return;
        }
        if (task.depth >= maxDepth) {
            shadeDiffuse(scene, image, task, xData);
            scene.metrics.record("maxDepth");
        }

        const auto& material = scene.materials[xData.materialIndex];
        switch (material.type) {
        case Material::Type::DIFFUSE:
            shadeDiffuse(scene, image, task, xData);
            break;
        case Material::Type::CONSTANT:
            shadeConstant(scene, image, task, xData);
            break;
        case Material::Type::REFLECTIVE:
            shadeReflective(scene, task, xData);
            break;
        case Material::Type::REFRACTIVE:
            shadeRefractive(scene, task, xData);
            break;
        case Material::Type::DEBUG:
            shadeNormal(image, task, xData);
            break;
        }
    }

    // TODO: abstract lerp away
    void shadeVoid(const Scene& scene, Image& image, const TraceTask& task) const
    {
        Vec3 unitColor = lerp(task.color, scene.bgColor, task.attenuation);
        image(task.pixelX, task.pixelY) = Color::fromUnit(unitColor);
    }

    void shadeConstant(const Scene& scene, Image& image, TraceTask& task, const Intersection& xData)
    {
        auto& material = scene.materials[xData.materialIndex];
        Vec3 unitColor = lerp(task.color, material.albedo, task.attenuation);
        image(task.pixelX, task.pixelY) = Color::fromUnit(unitColor);
    }

    void shadeReflective(const Scene& scene, TraceTask& task, Intersection& xData)
    {
        auto& material = scene.materials[xData.materialIndex];
        xData.p = xData.p + xData.n * shadowBias;
        Vec3 light = hitLight(scene, xData.p, xData.n);
        Vec3 diffuseComponent = multiply(light, material.albedo);
        task.ray.reflect(xData.p, xData.n);
        task.color = lerp(task.color, diffuseComponent, task.attenuation);
        task.attenuation *= material.reflectivity;
        ++task.depth; 
        traceQueue.push(task);
    }

    void shadeDiffuse(const Scene& scene, Image& image, TraceTask& task, Intersection& xData)
    {
        auto& material = scene.materials[xData.materialIndex];
        xData.p = xData.p + xData.n * shadowBias;
        Vec3 light = hitLight(scene, xData.p, xData.n);
        Vec3 diffuseComponent = multiply(light, material.albedo);
        Vec3 unitColor = lerp(task.color, diffuseComponent, task.attenuation);
        image(task.pixelX, task.pixelY) = Color::fromUnit(unitColor);
    }

    void shadeRefractive(const Scene& scene, TraceTask& task, Intersection& xData)
    {
        #ifndef NDEBUG
        auto oldRay = task.ray;
        #endif
        auto& material = scene.materials[xData.materialIndex];
        float enteringIor = material.ior; // the IOR of the material we're entering
        Vec3 facingN = xData.n; //todo make reference
        if (xData.type == IntersectionType::INSIDE_REFRACTIVE) {
            facingN = -xData.n;
            //  (simplification) exiting a refractive object means we're back to air
            enteringIor = 1.f; // todo: implement nested refractive objects
        }
        assert(dot(facingN, task.ray.direction) < -1e-6);

        auto diffuseP = xData.p + facingN * shadowBias;
        auto refractP = xData.p - facingN * refractionBias;
        Vec3 light = hitLight(scene, diffuseP, xData.n);
        Vec3 diffuseComponent = multiply(light, material.albedo);
        task.ray.refract(refractP, facingN, task.ior, enteringIor);
        task.ior = enteringIor;
        task.color = lerp(task.color, diffuseComponent, task.attenuation);
        task.attenuation *= material.transparency;
        ++task.depth; 
        traceQueue.push(task);

        // assert refractP is farther away from ray.origin than xData.p
        assert((refractP - oldRay.origin).lengthSquared() > (xData.p - oldRay.origin).lengthSquared());
    }

    void shadeNormal(Image& image, TraceTask& task, const Intersection& xData)
    {
        Vec3 unitColor = xData.n * 0.5f + Vec3{0.5f, 0.5f, 0.5f};
        image(task.pixelX, task.pixelY) = Color::fromUnit(unitColor);
    }

    Vec3 hitLight(const Scene& scene, const Vec3& p, const Vec3& n) const
    {
        Vec3 shade{ 0.f, 0.f, 0.f };
        for (const Light& light : scene.lights) {
            shade = shade + light.lightContrib(scene, p, n);
        }
        shade.clamp(0.f, 1.f);
        return shade;
    }

    /* hw3. For debugging camera Ray generation */
    Color traceImagePlane(const Scene& scene, const Ray& ray) const
    {
        float imagePlaneDist = 1.f;
        Vec3 p = ray.origin + ray.direction;
        Vec3 ortho = ray.origin + scene.camera.getDir(); // shortest vect. to img plane
        float rayProj = dot(ortho, p);

        float scale = imagePlaneDist / rayProj;
        p = p * scale;
        Intersection xData;
        xData.p = p;
        return shade_abs(xData);
    }

    Color shade_dbg_b(Vec3 p) const {
        uint8_t b = static_cast<uint8_t>(fabs(p.z + 1.0f) * 127.5f);
        return Color{ 5,5 , b };
    }

    Color shade_abs(const Intersection& xData) const {
        uint8_t r = static_cast<uint8_t>(fabs(xData.p.x * 100.f));
        uint8_t g = static_cast<uint8_t>(fabs(xData.p.y * 100.f));
        uint8_t b = static_cast<uint8_t>(fabs(xData.p.z * 100.f));

        return Color{ r, g, b };
    }

    //Color shade_normal(const Intersection& xData) const {
    //    uint8_t r = static_cast<uint8_t>(fabs(xData.n.x + 1.f) * 127.5f);
    //    uint8_t g = static_cast<uint8_t>(fabs(xData.n.y + 1.f) * 127.5f);
    //    uint8_t b = static_cast<uint8_t>(fabs(xData.n.z + 1.f) * 127.5f);

    //    return Color{ r, g, b };
    //}

    Color shade_perlin(Vec3 p) const
    {
        static const siv::PerlinNoise::seed_type seed = 123456u;
        static const siv::PerlinNoise perlin{ seed };

        const double noise = perlin.octave2D_01(p.x, p.y, 4);
        const uint8_t color = static_cast<uint8_t>(noise * 255);
        return Color{ color, color, color };
    }

    Color shade_uv(const Intersection& xData)
    {
        static const siv::PerlinNoise::seed_type seed = 123456u;
        static const siv::PerlinNoise perlin{ seed };

        const double noise = perlin.octave2D_01(xData.u, xData.v, 8, 0.8);
        float brightness = static_cast<uint8_t>(noise * 255);
        const uint8_t r = static_cast<uint8_t>(brightness * xData.n.x);
        const uint8_t g = static_cast<uint8_t>(brightness * xData.n.y);
        const uint8_t b = static_cast<uint8_t>(brightness * xData.n.z);
        return Color{ r, g, b };
    }
};

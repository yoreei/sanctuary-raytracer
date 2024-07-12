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
#include "ImageQueue.h"

#include "perlin-noise.h"

class Renderer {
public:
    size_t maxDepth = 3;
    ImageQueue imageQueue {0, 0, {0.f, 0.f, 0.f}};
    Renderer() {}

    /*
    * @parameter imageComponents: depth slices of the image, useful for debugging
    */
    void renderScene(const Scene& scene, Image& image, std::vector<Image>& imageComponents)
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

        imageQueue = {image.getWidth(), image.getHeight(), scene.bgColor};
        processTraceQueue(scene);
        std::cout<<"Flattening image"<<std::endl;
        ImageQueue imageQueueCopy = imageQueue;
        imageQueue.flatten(image);
        std::cout<<"imageQueue.maxDepth() = "<<imageQueue.maxDepth()<<std::endl;
        imageQueueCopy.slice(imageComponents);
        std::cout<<"Image flattened"<<std::endl;
        //processShadowQueue(scene, image);
        scene.metrics.stopTimer();
    }

    /*
    * DEBUGGING
    * Fires a single ray.
    */
    //void DBG_renderScene(const Scene& scene, Image& image, Ray ray = { {0.f, 0.f, 0.f}, {-1.f, -1.f, -1.f} })
    //{
    //    //ray.direction.normalize();
    //    ray.origin = { 0.f, 0.f, 0.f };
    //    ray.direction = { 0.f, 0.f, -1.f };
    //    TraceTask task = { ray, 0, 0, };
    //    traceQueue.push(task);
    //    imageQueue = {1, 1};
    //    processTraceQueue(scene);
    //    imageQueue.flatten(image);
    //    std::cout << image(0, 0) << std::endl;
    //    std::cout << std::endl;
    //}

    std::queue<TraceTask> traceQueue {};
    float shadowBias = 0.001f;
    float refractionBias = 0.001f;
private:

    void processTraceQueue(const Scene& scene)
    {
        while (!traceQueue.empty()) {
            TraceTask task = traceQueue.front();
            Ray& ray = task.ray;
            traceQueue.pop();
            Intersection xData{};
            scene.intersect(ray, xData);
            processXData(scene, task, xData);
        }
    }

    void processXData(const Scene& scene, TraceTask& task, Intersection& xData) {
        if (!xData.successful()) {
            return;
        }
        if (task.depth >= maxDepth) {
            //shadeDiffuse(scene, task, xData);
            scene.metrics.record("maxDepth");
            std::cout<<"imageQueue.maxDepth() = "<<imageQueue.maxDepth()<<std::endl;
            return;
        }

        const auto& material = scene.materials[xData.materialIndex];
        switch (material.type) {
        case Material::Type::DIFFUSE:
            shadeDiffuse(scene, task, xData);
            break;
        case Material::Type::CONSTANT:
            shadeConstant(scene, task, xData);
            break;
        case Material::Type::REFLECTIVE:
            shadeReflective(scene, task, xData);
            break;
        case Material::Type::REFRACTIVE:
            shadeRefractive(scene, task, xData);
            break;
        case Material::Type::DEBUG:
            shadeNormal(task, xData);
            break;
        case Material::Type::VOID:
            throw std::invalid_argument("not handled");
            break;
        }
    }

    void shadeConstant(const Scene& scene, TraceTask& task, const Intersection& xData)
    {
        auto& material = scene.materials[xData.materialIndex];
        imageQueue(task.pixelX, task.pixelY).push({material.albedo, task.attenuation});
    }

    void shadeDiffuse(const Scene& scene, const TraceTask& task, Intersection& xData)
    {
#ifndef NDEBUG
        size_t pixelDepth = imageQueue(task.pixelX, task.pixelY).size();
#endif
        auto& material = scene.materials[xData.materialIndex];
        xData.p = xData.p + xData.n * shadowBias;
        Vec3 light = hitLight(scene, xData.p, xData.n); // overexposure x,y,z > 1.f
        Vec3 overexposedColor = multiply(light, material.albedo);
        Vec3 diffuseComponent = clampOverexposure(overexposedColor);
        imageQueue(task.pixelX, task.pixelY).push({diffuseComponent, task.attenuation});
        assert(imageQueue(task.pixelX, task.pixelY).size() == pixelDepth + 1);
        //assert(imageQueue(task.pixelX, task.pixelY).size() == 1);
    }

    void shadeReflective(const Scene& scene, TraceTask& task, Intersection& xData)
    {
        xData.p = xData.p + xData.n * shadowBias;
        shadeDiffuse(scene, task, xData);

        auto& material = scene.materials[xData.materialIndex];
        task.ray.reflect(xData.p, xData.n);
        task.attenuation *= material.reflectivity;
        ++task.depth;
        traceQueue.push(task);
    }

    void shadeRefractive(const Scene& scene, TraceTask& task, Intersection& xData)
    {
#ifndef NDEBUG
        auto oldRay = task.ray;
#endif
        ++task.depth;
        shadeDiffuse(scene, task, xData);

        auto& material = scene.materials[xData.materialIndex];
        float enteringIor = material.ior; // the IOR of the material we're entering
        Vec3 facingN = xData.n; //todo make reference
        if (xData.type == IntersectionType::INSIDE_REFRACTIVE) {
            facingN = -xData.n;
            //  (simplification) exiting a refractive object means we're back to air
            enteringIor = 1.f; // todo: implement nested refractive objects
        }
        assert(dot(facingN, task.ray.direction) < -1e-6);

        Intersection xDataReflective = xData;
        TraceTask reflectiveTask = task;
        xDataReflective.p = xData.p + facingN * shadowBias;
        shadeReflective(scene, reflectiveTask, xDataReflective);

        TraceTask& refractionTask = task;
        Vec3 refractP = xData.p - facingN * refractionBias;
        bool hasRefraction = refractionTask.ray.refract(refractP, facingN, task.ior, enteringIor);
        if (hasRefraction) {
            refractionTask.attenuation *= material.transparency;
            refractionTask.ior = enteringIor;
            traceQueue.push(refractionTask);
        }
        // TODO: optimize copying

#ifndef NDEBUG
        // assert refractP is farther away from ray.origin than xData.p
        Vec3 op = xData.p - oldRay.origin;
        float opLen = op.lengthSquared();
        Vec3 oR = xDataReflective.p - oldRay.origin;
        float oRLen = oR.lengthSquared();
        assert(oRLen < opLen + 10 * epsilon);
#endif
    }

    void shadeNormal(TraceTask& task, const Intersection& xData)
    {
        Vec3 unitColor = xData.n * 0.5f + Vec3{0.5f, 0.5f, 0.5f};
        imageQueue(task.pixelX, task.pixelY).push({unitColor, task.attenuation});
    }

    Vec3 hitLight(const Scene& scene, const Vec3& p, const Vec3& n) const
    {
        Vec3 shade{ 0.f, 0.f, 0.f };
        for (const Light& light : scene.lights) {
            shade = shade + light.lightContrib(scene, p, n);
        }

        // shade.clamp(0.f, 1.f); // comment out for overexposure?
        return shade;
    }

    /*
    * @param overexposure: x,y,z > 1.f
    * @return: x,y,z clamped to 1.f
    */
    Vec3 clampOverexposure(const Vec3& overexposed) const {
       return clampOverexposureSlow(overexposed);
    }

    Vec3 clampOverexposureFast(const Vec3& overexposed) const {
        Vec3 rest = overexposed - Vec3{ 1.f, 1.f, 1.f };
        rest.clamp(0.f, FLT_MAX); 
        float scalar = Utils::max(rest.x, rest.y, rest.z) / 3;
        Vec3 result = overexposed + Vec3 { scalar, scalar, scalar};
        result.clamp(0.f, 1.f);
        return result;
    }

    // TODO: benchmark and try to optimize
    Vec3 clampOverexposureSlow(const Vec3& overexposed) const {
        Vec3 rest = overexposed - Vec3{ 1.f, 1.f, 1.f };
        rest.clamp(0.f, FLT_MAX);

        // Using a logarithmic approach to make the change more gradual
        Vec3 adjusted = Vec3 {
            log10(1.0f + rest.x),
            log10(1.0f + rest.y),
            log10(1.0f + rest.z)
        };

        Vec3 result = overexposed - adjusted;
        result.clamp(0.f, 1.f);
        return result;
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

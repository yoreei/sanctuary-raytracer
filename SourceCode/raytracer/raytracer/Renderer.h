#pragma once
#include <algorithm>
#include <tuple>
#include <queue>
#include <cmath>

#include "Camera.h"
#include "CRTTypes.h"
#include "Image.h"
#include "Scene.h"
#include "Triangle.h"
#include "Tasks.h"
#include "TraceHit.h"
#include "ImageQueue.h"

#include "perlin-noise.h"

namespace Timers {
    static constexpr const char* all = "RENDER_ALL";
    static constexpr const char* generateQueue = "RENDER_GENERATE_QUEUE";
    static constexpr const char* processQueue = "RENDER_PROCESS_QUEUE";
    static constexpr const char* flatten = "RENDER_FLATTEN";
} // namespace Timers

class Renderer {
public:
    size_t maxDepth = 16;
    float bias = 0.001f;
    bool debugSingleRay = false;
    bool debugLight = false;
    bool debugImageQueue = true;
private:
    ImageQueue imageQueue{ 0, 0, {0.f, 0.f, 0.f} };
    std::queue<TraceTask> traceQueue {};
    std::shared_ptr<Scene> scene;
public:
    Renderer(std::shared_ptr<Scene> scene) : scene(scene) {}

    /*
    * @parameter imageComponents: depth slices of the image, useful for debugging
    */
    void renderScene(Image& image, std::vector<Image>& imageComponents)
    {
        // TODO perf: reserve space for the queue
        scene->metrics.startTimer(Timers::all);
        if (debugLight) {
            scene->showLightDebug();
        }

        // Prepare Primary Queue
        if (debugSingleRay) {
            //Ray ray = scene->camera.rayFromPixel(image, 150, 140);
            Ray ray = { {0.f, 0.f, 0.f}, {0.f, 0.f, -1.f} };
            TraceTask task = { ray, 0, 0 };
            traceQueue.push(task);
        }
        else {
            scene->metrics.startTimer(Timers::generateQueue);
            for (int y = 0; y < image.getHeight(); ++y) {
                for (int x = 0; x < image.getWidth(); ++x) {
                    scene->camera.emplaceTask(image, x, y, traceQueue);
                }
            }
            scene->metrics.stopTimer(Timers::generateQueue);
        }
        imageQueue = { image.getWidth(), image.getHeight(), scene->bgColor };

        scene->metrics.startTimer(Timers::processQueue);
        processTraceQueue();
        scene->metrics.stopTimer(Timers::processQueue);
        flattenImage(image, imageComponents);

        scene->metrics.stopTimer(Timers::all);
    }
private:

    void processTraceQueue()
    {
        while (!traceQueue.empty()) {
            TraceTask task = traceQueue.front();
            Ray& ray = task.ray;
            traceQueue.pop();
            TraceHit hit{};
            scene->intersect(ray, hit);
            processXData(task, hit);
        }
    }

    void processXData(TraceTask& task, TraceHit& hit) {
        if (!hit.successful()) {
            return;
        }
        if (task.depth >= maxDepth) {
            scene->metrics.record("REACHED_MAX_DEPTH");
            return;
        }

        const auto& material = scene->materials[hit.materialIndex];
        switch (material.type) {
        case Material::Type::DIFFUSE:
            shadeDiffuse(task, hit);
            scene->metrics.record("HIT_MATERIAL_DIFFUSE");
            break;
        case Material::Type::CONSTANT:
            shadeConstant(task, hit);
            scene->metrics.record("HIT_MATERIAL_CONSTANT");
            break;
        case Material::Type::REFLECTIVE:
            shadeReflective(task, hit);
            scene->metrics.record("HIT_MATERIAL_REFLECTIVE");
            break;
        case Material::Type::REFRACTIVE:
            shadeRefractive(task, hit);
            scene->metrics.record("HIT_MATERIAL_REFRACTIVE");
            break;
        case Material::Type::DEBUG_NORMAL:
            shadeNormal(task, hit);
            // shadeUv(task, hit);
            scene->metrics.record("HIT_MATERIAL_DEBUG_NORMAL");
            break;
        case Material::Type::VOID:
            throw std::invalid_argument("not handled");
            break;
        }
    }

    void shadeConstant(TraceTask& task, const TraceHit& hit)
    {
        auto& material = scene->materials[hit.materialIndex];
        if (task.weight > epsilon) {
            imageQueue(task.pixelX, task.pixelY).push({ material.albedo, task.weight });
        }
    }

    void shadeDiffuse(const TraceTask& task, const TraceHit& hit)
    {
        if (task.weight < epsilon) {
            return;
        }

        auto& material = scene->materials[hit.materialIndex];
        Vec3 light = hitLight(hit.biasP(bias), hit.n); // overexposure x,y,z > 1.f
        Vec3 overexposedColor = multiply(light, material.albedo);
        Vec3 diffuseComponent = clampOverexposure(overexposedColor);
        imageQueue(task.pixelX, task.pixelY).push({ diffuseComponent, task.weight });
        assert(imageQueue(task.pixelX, task.pixelY).size() > 0);
    }

    void shadeReflective(TraceTask& task, const TraceHit& hit)
    {
        auto& material = scene->materials[hit.materialIndex];
        float originalWeight = task.weight;
        task.weight = originalWeight * (1.f - material.reflectivity);
        shadeDiffuse(task, hit);

        task.weight = originalWeight * material.reflectivity;
        if (task.weight > epsilon) {
            task.ray.reflect(hit.biasP(bias), hit.n);
            ++task.depth;
            traceQueue.push(task);
        }
    }

    void shadeRefractive(TraceTask& task, TraceHit& hit)
    {
#ifndef NDEBUG
        auto oldRay = task.ray;
#endif

        auto& material = scene->materials[hit.materialIndex];
        float enteringIor = material.ior; // the IOR of the material we're entering
        enteringIor *= 2; // TODO: why does it look better with this?
        if (hit.type == TraceHitType::INSIDE_REFRACTIVE) {
            hit.n = -hit.n; // let n always face ray
            //  (simplification) exiting a refractive object means we're back to air
            enteringIor = 1.f; // todo: implement nested refractive objects
            scene->metrics.record("shadeRefractive_INSIDE");
        }
        else {
            scene->metrics.record("shadeRefractive_OUTSIDE");
        }
        assert(dot(hit.n, task.ray.direction) < -1e-6);

        TraceTask& refractionTask = task;
        TraceTask reflectiveTask = task;
        bool hasRefraction = refractionTask.ray.refract(hit.biasP(-bias), hit.n, task.ior, enteringIor);

        if (hasRefraction) {
             float fresnelFactor = schlickApprox(task.ray.direction, hit.n, task.ior, enteringIor);

            refractionTask.weight *= (1.f - fresnelFactor);
            if (refractionTask.weight > epsilon) {
                ++refractionTask.depth;
                refractionTask.ior = enteringIor;
                traceQueue.push(refractionTask);
            }

            reflectiveTask.weight *= fresnelFactor;
            shadeReflective(reflectiveTask, hit); // shadeReflective takes care of shadeDiffuse

            scene->metrics.record("shadeRefractive_REFRACTED_YES");
        }
        else {
            shadeReflective(reflectiveTask, hit); // shadeReflective takes care of shadeDiffuse
            scene->metrics.record("shadeRefractive_REFRACTED_NO(TIR)");
        }

#ifndef NDEBUG
        // assert refractP is farther away from ray.origin than hit.p
        Vec3 oP = hit.p - oldRay.origin;
        float oPLen = oP.lengthSquared();
        Vec3 oReflect = hit.biasP(bias) - oldRay.origin;
        float oReflectLen = oReflect.lengthSquared();
        assert(oReflectLen < oPLen + 10 * epsilon);
#endif
    }
    /*
    * return: non-clamped hit light color, summed from all lights in the scene
    */
    Vec3 hitLight(const Vec3& p, const Vec3& n) const
    {
        Vec3 shade{ 0.f, 0.f, 0.f };
        for (const Light& light : scene->lights) {
            shade = shade + light.lightContrib(*scene, p, n);
        }

        return shade;
    }

    /*
    * @param overexposure: x,y,z > 1.f
    * @return: x,y,z clamped to 1.f
    */
    Vec3 clampOverexposure(const Vec3& overexposed) const {
        scene->metrics.startTimer("slow_overexposure");
        Vec3 slow = clampOverexposureSlow(overexposed);
        scene->metrics.startTimer("slow_overexposure");
        return slow;
    }

    Vec3 clampOverexposureSlow(const Vec3& overexposed) const {
        Vec3 rest = overexposed - Vec3{ 1.f, 1.f, 1.f };
        rest.clamp(0.f, FLT_MAX);

        // Using a logarithmic approach to make the change more gradual
        Vec3 adjusted = Vec3{
            log10(1.0f + rest.x),
            log10(1.0f + rest.y),
            log10(1.0f + rest.z)
        };

        Vec3 result = overexposed - adjusted;
        result.clamp(0.f, 1.f);
        return result;
    }

    /*
    * @return: fresnel factor
    */
    float schlickApprox(const Vec3& i, const Vec3& n, float n1, float n2) const {
        n1; // clear unused param warning
        n2;
        return schlickApproxVladi(i, n);
    }
    /*
    * @return: fresnel factor
    */
    float schlickApproxWikipedia(const Vec3& i, const Vec3& n, float n1, float n2) const {
        // assumption: n is always facing i
        float cosTheta = -dot(i, n);
        float F0 = std::powf((n1 - n2) / (n1 + n2), 2);
        return F0 + (1.f - F0) * std::powf(1.f - cosTheta, 5);
    }

    /*
    * @return: fresnel factor
    */
    float schlickApproxVladi(const Vec3& i, const Vec3& n) const {
        float cosTheta = dot(i, n);
        return 0.5f * powf(1.f + cosTheta, 5); // vladi's value
    }

    /* hw3. For debugging camera Ray generation */
    Color traceImagePlane(const Ray& ray) const
    {
        float imagePlaneDist = 1.f;
        Vec3 p = ray.origin + ray.direction;
        Vec3 ortho = ray.origin + scene->camera.getDir(); // shortest vect. to img plane
        float rayProj = dot(ortho, p);

        float scale = imagePlaneDist / rayProj;
        p = p * scale;
        TraceHit hit;
        hit.p = p;
        return shadePosition(hit);
    }

    Color shade_dbg_b(Vec3 p) const {
        uint8_t b = static_cast<uint8_t>(fabs(p.z + 1.0f) * 127.5f);
        return Color{ 5,5 , b };
    }

    Color shadePosition(const TraceHit& hit) const {
        uint8_t r = static_cast<uint8_t>(fabs(hit.p.x * 100.f));
        uint8_t g = static_cast<uint8_t>(fabs(hit.p.y * 100.f));
        uint8_t b = static_cast<uint8_t>(fabs(hit.p.z * 100.f));

        return Color{ r, g, b };
    }

    Color shadePerlin(Vec3 p) const
    {
        static const siv::PerlinNoise::seed_type seed = 123456u;
        static const siv::PerlinNoise perlin{ seed };

        const double noise = perlin.octave2D_01(p.x, p.y, 4);
        const uint8_t color = static_cast<uint8_t>(noise * 255);
        return Color{ color, color, color };
    }

    void shadeUv(const TraceTask& task, const TraceHit& hit)
    {
        assert(hit.u <= 1.f + epsilon && hit.u >= -epsilon);
        assert(hit.v <= 1.f + epsilon && hit.u >= -epsilon);
        Vec3 unitColor = { hit.u, 0.f, hit.v };
        imageQueue(task.pixelX, task.pixelY).push({ unitColor, task.weight });
    }

    void shadeNormal(TraceTask& task, const TraceHit& hit)
    {
        Vec3 unitColor = hit.n * 0.5f + Vec3{0.5f, 0.5f, 0.5f};
        imageQueue(task.pixelX, task.pixelY).push({ unitColor, task.weight });
    }

    void flattenImage(Image& image, std::vector<Image>& imageComponents)
    {
        if (debugImageQueue) {
            ImageQueue imageQueueCopy = imageQueue;
            imageQueue.flatten(image);
            imageQueueCopy.slice(imageComponents);
        }
        else {
            scene->metrics.startTimer(Timers::flatten);
            imageQueue.flatten(image);
            scene->metrics.stopTimer(Timers::flatten);
        }
    }
};

#pragma once
#include <algorithm>
#include <tuple>
#include <queue>
#include <cmath>
#include <thread>
#include <functional>

#include "Camera.h"
#include "CRTTypes.h"
#include "Image.h"
#include "Scene.h"
#include "Triangle.h"
#include "TraceHit.h"
#include "ShadingSamples.h"

#include "perlin-noise.h"


class Renderer {
    struct Timers {
        static constexpr const char* all = "RENDER_ALL";
        static constexpr const char* generateQueue = "RENDER_GENERATE_QUEUE";
        static constexpr const char* processQueue = "RENDER_PROCESS_QUEUE";
        static constexpr const char* flatten = "RENDER_FLATTEN";
    };

public:
private:
    // TODO perf: reserve space for the queue
    using TraceQueue = std::queue<TraceTask>;
    ShadingSamples shadingSamples{ 0, 0, settings };
    std::vector<TraceQueue> queueBuckets {};

    // Dependencies
    const Scene* scene;
    const Settings* settings;
    Image* image;
    std::vector<Image>* auxImages;
public:
    Renderer(const Settings* settings, const Scene* scene, Image* image, std::vector<Image>* auxImages) :
        settings(settings),
        scene(scene),
        image(image),
        auxImages(auxImages) { }

    /*
    * @parameter imageComponents: depth slices of the image, useful for debugging
    */
    void render()
    {
        GSceneMetrics.startTimer(Timers::all);
        if (scene->getIsDirty()) throw std::invalid_argument("Scene is dirty, cannot render");

        // Prepare Primary Queue
        GSceneMetrics.startTimer(Timers::generateQueue);

        size_t numPixels = image->getWidth() * image->getHeight();
        if (numPixels % image->bucketSize != 0) throw std::invalid_argument("numBuckets must divide image size");
        size_t numBuckets = numPixels / image->bucketSize;

        queueBuckets.resize(numBuckets);
        for (size_t y = image->startPixelY; y < image->endPixelY; ++y) {
            for (size_t x = image->startPixelX; x < image->endPixelX; ++x) {
                size_t bucketId = (y * image->getWidth() + x) / image->bucketSize;
                TraceQueue& queue = queueBuckets[bucketId];
                scene->camera.emplaceTask(*image, x, y, queue);
            }
        }

        GSceneMetrics.stopTimer(Timers::generateQueue);
        shadingSamples = { image->getWidth(), image->getHeight(), settings };

        GSceneMetrics.startTimer(Timers::processQueue);
        launchBuckets();
        GSceneMetrics.stopTimer(Timers::processQueue);

        flattenImage();
        GSceneMetrics.stopTimer(Timers::all);
    }
private:

    void launchBuckets()
    {
        const size_t numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        std::atomic<size_t> nextQueueIndex{ numThreads };

        for (size_t threadId = 0; threadId < numThreads; ++threadId) {
            threads.emplace_back(&Renderer::workerThread, this, threadId, std::ref(nextQueueIndex));
        }

        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void workerThread(size_t initialIndex, std::atomic<size_t>& nextQueueIndex) {
        GSceneMetrics.reserveThread();
        size_t queueIndex = initialIndex;
        while (queueIndex < queueBuckets.size()) {
            processTraceQueue(queueBuckets[queueIndex]);
            queueIndex = nextQueueIndex.fetch_add(1);
        }
    }

    void processTraceQueue(TraceQueue& traceQueue)
    {
        while (!traceQueue.empty()) {
            TraceTask task = traceQueue.front();
            Ray& ray = task.ray;
            traceQueue.pop();
            TraceHit hit{};
            scene->intersect(ray, hit);
            processXData(task, hit, traceQueue);
        }
    }

    void processXData(TraceTask& task, TraceHit& hit, TraceQueue& traceQueue) {
        if (!hit.successful()) {
            if (task.weight > epsilon) {
                Vec3 skyColor = scene->cubemap.sample(task.ray);
                shadingSamples.addSample(task, skyColor);
            }
            return;
        }
        if (task.depth >= settings->maxDepth) {
            return;
        }

        const auto& material = scene->materials[hit.materialIndex];
        // TODO refactor these
        switch (material.type) {
        case Material::Type::DIFFUSE:
            shadeDiffuse(task, hit);
            break;
        case Material::Type::CONSTANT:
            shadeConstant(task, hit);
            break;
        case Material::Type::REFLECTIVE:
            shadeReflective(task, hit, traceQueue);
            break;
        case Material::Type::REFRACTIVE:
            shadeRefractive(task, hit, traceQueue);
            break;
        case Material::Type::DEBUG_NORMAL:
            shadeNormal(task, hit);
            break;
        case Material::Type::DEBUG_UV:
            shadeUv(task, hit);
            break;
        case Material::Type::DEBUG_BARY:
            shadeBary(task, hit);
            break;
        case Material::Type::VOID:
            throw std::invalid_argument("not handled");
            break;
        }
        postProcessTraceColor(task, hit);
    }

    void postProcessTraceColor(TraceTask& task, const TraceHit& hit)
    {
        task;
        if (hit.traceColor.equal(Vec3{ 0.f, 0.f, 0.f })) {
            return;
        }
        throw std::runtime_error("TODO not implemented");

        //shadingSamples.addSample(task, hit.traceColor);
    }

    void shadeSimpleSky(const TraceTask& task, const TraceHit& hit) {
        hit;
        GSceneMetrics.record("ShadeSky");
        float t = 0.5f * (task.ray.direction.y + 1.f);
        Vec3 white{ 1.f, 1.f, 1.f };
        Vec3 blue{ 0.5f, 0.7f, 1.f };
        Vec3 skyColor = lerp(white, blue, t);

        if (task.weight > epsilon) {
            shadingSamples.addSample(task, skyColor, BlendType::ADDITIVE);
        }
    }

    void shadeConstant(TraceTask& task, const TraceHit& hit)
    {
        auto& material = scene->materials[hit.materialIndex];
        if (task.weight > epsilon) {
            shadingSamples.addSample(task, material.getAlbedo(*scene, hit));
        }
    }

    void shadeDiffuse(const TraceTask& task, const TraceHit& hit)
    {
        if (task.weight < epsilon) {
            return;
        }

        const Material& material = scene->materials[hit.materialIndex];
        Vec3 albedo = material.getAlbedo(*scene, hit);
        Vec3 light = hitLight(hit.biasP(settings->bias), hit.n); // overexposure x,y,z > 1.f
        Vec3 overexposedColor = multiply(light, albedo);
        Vec3 diffuseComponent = clampOverexposure(overexposedColor);
        shadingSamples.addSample(task, diffuseComponent, BlendType::NORMAL);
        assert(shadingSamples(task.pixelX, task.pixelY).size() > 0);
    }

    void shadeReflective(TraceTask& task, const TraceHit& hit, TraceQueue& traceQueue)
    {
        // TODO remove comments 
        auto& material = scene->materials[hit.materialIndex];
        //float originalWeight = task.weight;
        //task.weight = originalWeight * (1.f - material.reflectivity);
        shadeDiffuse(task, hit);

        //task.weight = originalWeight * material.reflectivity;
        task.weight *= material.reflectivity;
        if (task.weight > epsilon) {
            task.ray.reflect(hit.biasP(settings->bias), hit.n);
            ++task.depth;
            traceQueue.push(task);
        }
    }

    void shadeRefractive(TraceTask& task, TraceHit& hit, TraceQueue& traceQueue)
    {
#ifndef NDEBUG
        auto oldRay = task.ray;
#endif

        auto& material = scene->materials[hit.materialIndex];
        float etai = 1, etat = material.ior;
        etat *= 1; // TODO: why does it look better with this?
        if (hit.type == TraceHitType::INSIDE_REFRACTIVE) {
            hit.n = -hit.n;
            std::swap(etai, etat);
            GSceneMetrics.record("shadeRefractive_INSIDE");
        }
        else {
            GSceneMetrics.record("shadeRefractive_OUTSIDE");
        }

        TraceTask& refractionTask = task;
        TraceTask reflectiveTask = task;
        shadeReflective(reflectiveTask, hit, traceQueue); // shadeReflective takes care of shadeDiffuse

        bool hasRefraction = refractionTask.ray.refractSP(hit.biasP(-settings->bias), hit.n, etai, etat);
        if (hasRefraction) {
            float fresnelFactor = schlickApprox(task.ray.direction, hit.n, etai, etat);

            refractionTask.weight *= (1.f - fresnelFactor);
            if (refractionTask.weight > epsilon) {
                ++refractionTask.depth;
                refractionTask.ior = etat;
                traceQueue.push(refractionTask);
            }

            // reflectiveTask.weight *= fresnelFactor;
            // shadeReflective(reflectiveTask, hit, traceQueue); // shadeReflective takes care of shadeDiffuse

            GSceneMetrics.record("shadeRefractive_REFRACTED_YES");
        }
        // else {
            // GSceneMetrics.record("shadeRefractive_REFRACTED_NO(TIR)");
        //}

#ifndef NDEBUG
        // assert refractP is farther away from ray.origin than hit.p
        Vec3 oP = hit.p - oldRay.origin;
        float oPLen = oP.lengthSquared();
        Vec3 oReflect = hit.biasP(settings->bias) - oldRay.origin;
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
        Vec3 slow = clampOverexposureSlow(overexposed);
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

    void shadeBary(const TraceTask& task, const TraceHit& hit)
    {
        assert(hit.baryU <= 1.f && hit.baryU >= 0);
        assert(hit.baryV <= 1.f && hit.baryV >= 0);
        Vec3 unitColor = { hit.baryU, 0.f, hit.baryV };
        shadingSamples.addSample(task, unitColor);
    }

    void shadeUv(const TraceTask& task, const TraceHit& hit)
    {
        assert(hit.u <= 1.f && hit.u >= 0);
        assert(hit.v <= 1.f && hit.u >= 0);
        Vec3 unitColor = { hit.u, 0.f, hit.v };
        shadingSamples.addSample(task, unitColor);
    }

    void shadeNormal(TraceTask& task, const TraceHit& hit)
    {
        Vec3 unitColor = hit.n * 0.5f + Vec3{0.5f, 0.5f, 0.5f};
        shadingSamples.addSample(task, unitColor);
    }

    void flattenImage()
    {
        if (settings->enableShadingSamples) {
            ShadingSamples shadingSamplesCopy = shadingSamples;
            shadingSamples.flatten(*image);
            shadingSamplesCopy.slice(*auxImages);
        }
        else {
            GSceneMetrics.startTimer(Timers::flatten);
            shadingSamples.flatten(*image);
            GSceneMetrics.stopTimer(Timers::flatten);
        }
    }
};

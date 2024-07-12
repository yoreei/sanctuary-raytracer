#pragma once
#include <algorithm>
#include <tuple>
#include <queue>

#include "Camera.h"
#include "CRTTypes.h"
#include "Image.h"
#include "Scene.h"
#include "Triangle.h"

#include "perlin-noise.h"

class Renderer {
public:
    Renderer() {}

    void renderScene(const Scene& scene, Image& image)
    {
        // Prepare Primary Queue
        scene.metrics.startTimer();
        Ray ray;
        for (int y = 0; y < image.getHeight(); ++y) {
            for (int x = 0; x < image.getWidth(); ++x) {
                scene.camera.emplacePrimaryRay(image, x, y, primaryQueue);
            }
        }

        processPrimaryQueue(scene, image);
        processShadowQueue(scene, image);
        scene.metrics.stopTimer();
    }

    ///*
    //* Fires a single ray. For Debugging traceRay
    //*/
    //void _renderSingle(const Scene& scene, Image& image, Ray ray = {{0.f, 0.f, 0.f}, {-1.f, -1.f, -1.f}})
    //{
    //        ray.direction = ray.direction.normalize();
    //        traceScene(scene, ray);
    //}

    std::queue<PixelRay> primaryQueue {};
    std::queue<ShadowRay> shadowQueue {};
    bool bProcessShadows = true;
private:

    void processPrimaryQueue(const Scene& scene, Image& image)
    {
        while (!primaryQueue.empty()) {
            PixelRay ray = primaryQueue.front();
            primaryQueue.pop();
            Triangle::IntersectionData xData, xDataBest;
            for (const Triangle& tri : scene.triangles) {
                // TODO: Separate plane intersection & triangle uv intersection tests for perf.
                Intersection x = tri.intersect(scene.vertices, ray, xData); 
                if ( xData.t < xDataBest.t && x == Intersection::SUCCESS) {
                    xDataBest = xData;
                }

                scene.metrics.record(toString(x));
            }

            if (xDataBest.intersectionSuccessful() && bProcessShadows) {
                for (const Light& light : scene.lights) {
                    Vec3 point = xDataBest.p + xDataBest.n * 0.001f;
                    ShadowRay shadowRay {ray.origin, point, xDataBest.n, ray.pixelX, ray.pixelY};
                    shadowQueue.push(shadowRay);
                }
            }
            else if (xDataBest.intersectionSuccessful() && !bProcessShadows) {
                image(ray.pixelX, ray.pixelY) = shade_normal(xDataBest);
            }
            else {
                image(ray.pixelX, ray.pixelY) = scene.bgColor;
            }
        }
    }

    void processShadowQueue(const Scene& scene, Image& image)
    {
        while (!shadowQueue.empty()) {
            ShadowRay shadowRay = shadowQueue.front();
            shadowQueue.pop();
            Vec3 contrib {0.f, 0.f, 0.f};
            for (const Light& light : scene.lights) {
                contrib = contrib + light.lightContrib(scene, shadowRay.point, shadowRay.normal);
            }

            contrib.clamp(0.f, 1.f);
            image(shadowRay.pixelX, shadowRay.pixelY) = Color::fromUnit(contrib);
        }
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
        Triangle::IntersectionData xData;
        xData.p = p;
        return shade_abs(xData);
    }

    Color shade_dbg_b(Vec3 p) const {
        uint8_t b = static_cast<uint8_t>(fabs(p.z + 1.0f) * 127.5f);
        return Color{ 5,5 , b };
    }

    Color shade_abs(const Triangle::IntersectionData& xData) const {
        uint8_t r = static_cast<uint8_t>(fabs(xData.p.x * 100.f));
        uint8_t g = static_cast<uint8_t>(fabs(xData.p.y * 100.f));
        uint8_t b = static_cast<uint8_t>(fabs(xData.p.z * 100.f));

        return Color{ r, g, b };
    }

    Color shade_normal(const Triangle::IntersectionData& xData) const {
        uint8_t r = static_cast<uint8_t>(fabs(xData.n.x + 1.f) * 127.5f);
        uint8_t g = static_cast<uint8_t>(fabs(xData.n.y + 1.f) * 127.5f);
        uint8_t b = static_cast<uint8_t>(fabs(xData.n.z + 1.f) * 127.5f);

        return Color{ r, g, b };
    }

    Color shade_perlin(Vec3 p) const
    {
        static const siv::PerlinNoise::seed_type seed = 123456u;
	    static const siv::PerlinNoise perlin{ seed };
	
		const double noise = perlin.octave2D_01(p.x, p.y, 4);
        const uint8_t color = static_cast<uint8_t>(noise * 255);
        return Color{ color, color, color };
    }

    Color shade_uv(const Triangle::IntersectionData& xData)
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

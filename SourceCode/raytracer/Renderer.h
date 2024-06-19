#pragma once
#include <algorithm>
#include <tuple>

#include "CRTTypes.h"
#include "Camera.h"
#include "perlin-noise.h"
#include "Scene.h"
#include "Triangle.h"
#include "RendererMetrics.h"
#include "Image.h"

class Renderer {
public:
    Renderer(Scene* _scene)
        : scene(_scene){}

    void renderScene(Image& image)
    {
        metrics.startTimer();
        for (int y = 0; y < image.height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                Ray ray = scene->camera.generateRay(image, x, y);
                image(x, y) = traceScene(ray); // to debug camera: traceImagePlane(ray)
            }
        }
        metrics.stopTimer();
    }

    /*
    * Fires a single ray. For Debugging traceRay
    */
    void _renderSingle(Image& image, Ray ray = {{0.f, 0.f, 0.f}, {-1.f, -1.f, -1.f}})
    {
            ray.direction = ray.direction.normalize();
            traceScene(ray);
    }

    RendererMetrics metrics {};
private:
    Scene* scene;

    Color traceScene(const Ray& ray)
    {
        Triangle::IntersectionData xData, xDataBest;
        bool bSuccess = false;
        for (const Triangle& tri : scene->triangles) {
            Intersection x = tri.intersect(ray, xData); // TODO: Separate plane intersection & triangle uv intersection tests for perf.
            if ( xData.t < xDataBest.t && x == Intersection::SUCCESS) {
                xDataBest = xData;
                bSuccess = true;
            }

            metrics.record(toString(x));
        }

        return bSuccess ? shade_abs(xDataBest) : scene->bgColor;
    }

    /* hw3. For debugging camera Ray generation */
    Color traceImagePlane(const Ray& ray) const
    {
        Vec3 p = ray.origin + ray.direction;
        float scale = -1.f / p.z; // TODO: ray.direction.z / p.z
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
        metrics.record("normal: " + xData.n.toString());
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

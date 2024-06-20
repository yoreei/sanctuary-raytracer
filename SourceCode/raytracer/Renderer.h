#pragma once
#include <algorithm>
#include <tuple>

#include "Camera.h"
#include "CRTTypes.h"
#include "Image.h"
#include "RendererMetrics.h"
#include "Scene.h"
#include "Triangle.h"

#include "perlin-noise.h"

class Renderer {
public:
    Renderer() {}

    void renderScene(const Scene& scene, Image& image)
    {
        metrics.startTimer();
        for (int y = 0; y < image.height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                Ray ray = scene.camera.generateRay(image, x, y);
                image(x, y) = traceScene(scene, ray);
                //traceImagePlane(ray) // to debug camera
            }
        }
        metrics.stopTimer();
    }

    /*
    * Fires a single ray. For Debugging traceRay
    */
    void _renderSingle(const Scene& scene, Image& image, Ray ray = {{0.f, 0.f, 0.f}, {-1.f, -1.f, -1.f}})
    {
            ray.direction = ray.direction.normalize();
            traceScene(scene, ray);
    }

    RendererMetrics metrics {};
private:

    Color traceScene(const Scene& scene, const Ray& ray)
    {
        Triangle::IntersectionData xData, xDataBest;
        bool bSuccess = false;
        for (const Triangle& tri : scene.triangles) {
            Intersection x = tri.intersect(ray, xData); // TODO: Separate plane intersection & triangle uv intersection tests for perf.
            if ( xData.t < xDataBest.t && x == Intersection::SUCCESS) {
                xDataBest = xData;
                bSuccess = true;
            }

            metrics.record(toString(x));
        }

        return bSuccess ? shade_abs(xDataBest) : scene.bgColor;
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

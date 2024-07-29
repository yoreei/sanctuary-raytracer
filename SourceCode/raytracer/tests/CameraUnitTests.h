#pragma once

#include <cassert>

#include "Camera.h"
#include "UnitTestData.h"
#include "Image.h"
#include "CRTTypes.h"

namespace CameraUnitTests
{
    void run()
    {
        Camera cam {};
        cam.setDir({0.f, 0.f, -1.f});
        assert(cam.mat.isOrthonormal());
        assert(cam.getUp().equal(Vec3{0.f, 1.f, 0.f}));

        cam.setDir({0.f, 1.f, 0.f});
        assert(cam.mat.isOrthonormal());

        cam.setDir({1.f, 0.f, 0.f});
        assert(cam.mat.isOrthonormal());

        /*
        Camera cam;
        UnitTestData::loadCamera1(cam);
        TraceTask task {};

        // Box aspect ratio
        Image img(3, 3);
        std::queue<TraceTrask> q;

        cam.emplacePrimaryRay(img, 0, 0, q);
        ray = q.front();
        assert(ray.origin.equal({ 0.f, 0.f, 0.f }));
        assert(ray.getDirection().equal({ -0.485071242f, 0.485071242f, -0.727606893f }));

        cam.emplacePrimaryRay(img, 1, 0, q);
        ray = q.front();
        assert(ray.getDirection().equal({ 0.00000000f, 0.554700196f, -0.832050323f }));

        cam.emplacePrimaryRay(img, 2, 0, q);
        ray = q.front();
        assert(ray.getDirection().equal({ 0.485071242f, 0.485071242f, -0.727606893f }));

        cam.emplacePrimaryRay(img, 0, 1, q);
        ray = q.front();
        assert(ray.getDirection().equal({ -0.554700196f, 0.00000000f, -0.832050323f }));

        cam.emplacePrimaryRay(img, 1, 1, q);
        ray = q.front();
        assert(ray.getDirection().equal({ 0.00000000f, 0.00000000f, -1.00000000f }));

        cam.emplacePrimaryRay(img, 2, 1, q);
        ray = q.front();
        assert(ray.getDirection().equal({ 0.554700196f, 0.00000000f, -0.832050323f }));

        cam.emplacePrimaryRay(img, 0, 2, q);
        ray = q.front();
        assert(ray.getDirection().equal({ -0.485071242f, -0.485071242f, -0.727606893f }));

        cam.emplacePrimaryRay(img, 1, 2, q);
        ray = q.front();
        assert(ray.getDirection().equal({ 0.00000000f, -0.554700196f, -0.832050323f }));

        cam.emplacePrimaryRay(img, 2, 2, q);
        ray = q.front();
        assert(ray.getDirection().equal({ 0.485071242f, -0.485071242f, -0.727606893f }));
*/
    }
} // namespace CameraUnitTests
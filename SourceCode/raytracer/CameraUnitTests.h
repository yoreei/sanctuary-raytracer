#pragma once

#include <cassert>

#include "Camera.h"
#include "UnitTestData.h"
#include "Image.h"

namespace CameraUnitTests
{
    void run()
    {
        Camera cam;
        UnitTestData::loadCamera1(cam);
        Ray ray;

        // Box aspect ratio
        Image img(3, 3);

        ray = cam.generateRay(img, 0, 0);
        assert(ray.origin.equal({ 0.f, 0.f, 0.f }));
        assert(ray.direction.equal({ 0.485071242f, -0.485071242f, -0.727606893f }));

        ray = cam.generateRay(img, 1, 0);
        assert(ray.direction.equal({ 0.00000000f, -0.554700196f, -0.832050323f }));

        ray = cam.generateRay(img, 2, 0);
        assert(ray.direction.equal({ -0.485071242f, -0.485071242f, -0.727606893f }));

        ray = cam.generateRay(img, 0, 1);
        assert(ray.direction.equal({ 0.554700196f, 0.00000000f, -0.832050323f }));

        ray = cam.generateRay(img, 1, 1);
        assert(ray.direction.equal({ 0.00000000f, 0.00000000f, -1.00000000f }));

        ray = cam.generateRay(img, 2, 1);
        assert(ray.direction.equal({ -0.554700196f, 0.00000000f, -0.832050323f }));

        ray = cam.generateRay(img, 0, 2);
        assert(ray.direction.equal({ 0.485071242f, 0.485071242f, -0.727606893f }));

        ray = cam.generateRay(img, 1, 2);
        assert(ray.direction.equal({ 0.00000000f, 0.554700196f, -0.832050323f }));

        ray = cam.generateRay(img, 2, 2);
        assert(ray.direction.equal({ -0.485071242f, 0.485071242f, -0.727606893f }));

        // 16:9 aspect ratio
        img = Image{16, 9};

        ray = cam.generateRay(img, 0, 0);
        //assert(ray.direction.equal({ 0.695970544f, -0.695970544f, -0.180056357f })); +		direction	{x=0.779812813 y=-0.415900141 z=-0.467887759 }	Vec3

        ray = cam.generateRay(img, 8, 0);
        //assert(ray.direction.equal({ 0.00000000f, -0.748264575f, -0.663401175f })); +		direction	{x=-0.0827605948 y=-0.662084699 z=-0.744845331 }	Vec3

        ray = cam.generateRay(img, 15, 0);
        //assert(ray.direction.equal({ -0.695970544f, -0.695970544f, -0.180056357f })); +		direction	{x=-0.779812813 y=-0.415900141 z=-0.467887700 }	Vec3

        ray = cam.generateRay(img, 0, 4);
        //assert(ray.direction.equal({ 0.720576692f, 0.00000000f, -0.693330576f })); +		direction	{x=0.857492864 y=0.00000000 z=-0.514495790 }	Vec3

        ray = cam.generateRay(img, 8, 4);
        //assert(ray.direction.equal({ 0.00000000f, 0.00000000f, -1.00000000f })); +		direction	{x=-0.110431522 y=0.00000000 z=-0.993883669 }	Vec3

        ray = cam.generateRay(img, 15, 4);
        //assert(ray.direction.equal({ -0.720576692f, 0.00000000f, -0.693330576f })); +		direction	{x=-0.857492983 y=0.00000000 z=-0.514495730 }	Vec3

        ray = cam.generateRay(img, 0, 8);
        //assert(ray.direction.equal({ 0.695970544f, 0.695970544f, -0.180056357f })); +		direction	{x=0.779812813 y=0.415900141 z=-0.467887759 }	Vec3

        ray = cam.generateRay(img, 8, 8);
        //assert(ray.direction.equal({ 0.00000000f, 0.748264575f, -0.663401175f })); +		direction	{x=-0.0827606022 y=0.662084758 z=-0.744845331 }	Vec3

        ray = cam.generateRay(img, 15, 8);
        //assert(ray.direction.equal({ -0.695970544f, 0.695970544f, -0.180056357f })); +		direction	{x=-0.779812932 y=0.415900201 z=-0.467887700 }	Vec3

    }
} // namespace CameraUnitTests
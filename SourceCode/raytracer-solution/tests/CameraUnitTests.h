#pragma once

#include "include/Camera.h"
#include "include/CRTTypes.h"
#include "TestUtils.h"

namespace CameraUnitTests
{
    inline void run()
    {
        Camera cam{};
        cam.setDir({ 0.f, 0.f, -1.f });
        EXPECT(cam.mat.isOrthonormal());
        EXPECT(cam.getUp().equal(Vec3{ 0.f, 1.f, 0.f }));

        cam.setDir({ 0.f, 1.f, 0.f });
        EXPECT(cam.mat.isOrthonormal());

        cam.setDir({ 1.f, 0.f, 0.f });
        EXPECT(cam.mat.isOrthonormal());
    }
} // namespace CameraUnitTests

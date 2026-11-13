#pragma once

#include "include/CRTTypes.h"
#include "TestUtils.h"

namespace CRTTypesUnitTests
{
    inline void run()
    {
        Vec3 a{ 3.5f, 0.f, 0.f };
        Vec3 b{ 1.75f, 3.5f, 0.f };
        Vec3 c{};

        a.cross(b, c);
        EXPECT(c.equal({ 0.f, 0.f, 12.25f }));

        a = { 3.f, -3.f, 1.f };
        b = { 4.f, 9.f, 2.f };
        a.cross(b, c);
        EXPECT(c.equal({ -15.f, -2.f, 39.f }));
        EXPECT(fEqual(c.length(), 41.833f));

        // Parallel vectors have a zero cross product
        a = { 3.f, -3.f, 1.f };
        b = { -12.f, 12.f, -4.f };
        a.cross(b, c);
        EXPECT(c.equal({ 0.f, 0.f, 0.f }));
        EXPECT(fEqual(c.length(), 0.f));
    }
} // namespace CRTTypesUnitTests

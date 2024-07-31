#pragma once

#include <cassert>

#include "include/CRTTypes.h"

namespace CRTTypesUnitTests
{
    using namespace std;
    void run()
    {
        Vec3 A = { 3.5f, 0.f, 0.f };
        Vec3 B = { 1.75f, 3.5f, 0.f };
        Vec3 c;
        float l;
        
        A.cross(B, c);
        assert(c.equal({ 0.f, 0.f, 12.25f }));

        A = { 3.f,-3.f,1.f };
        B = { 4.f, 9.f, 2.f };
        A.cross(B, c);
        assert(c.equal({ -15.f, -2.f, 39.f }));

        l = c.length();
        assert(fEqual(l, 41.833f));

        A = { 3.f,-3.f,1.f };
        B = { -12.f, 12.f, -4.f };
        A.cross(B, c);
        assert(c.equal({ 0.f, 0.f, 0.f }));
        l = c.length();
        assert(fEqual(l, 0.f));
    }
} // namespace CRTTypesUnitTests

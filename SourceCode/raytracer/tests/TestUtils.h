#pragma once
#include "CRTTypes.h"
#include "Triangle.h"

Triangle createTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2)
{
    return Triangle{ {v0, v1, v2}, 0, 1, 2, 0 };
}

#pragma once
#include <memory>

#include "include/CRTTypes.h"
#include "json_fwd.h"

class SceneObject {
public:
    SceneObject() = default;
    SceneObject(const Vec3& pos, const Matrix3x3& mat) : pos(pos), mat(mat) {
    }
    Vec3 pos{ 0.f, 0.f, 0.f };
    Matrix3x3 mat = SceneObject::DefaultMatrix;

    void rotate(Vec3 rotVec)
    {
        rotVec;
        throw std::runtime_error("not impl");

    }

    void translateTo(Vec3 absPos)
    {
        absPos;
        throw std::runtime_error("not impl");
    }

    nlohmann::ordered_json toJson() const;
    inline const static Matrix3x3 DefaultMatrix = { {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, -1.f } };
};

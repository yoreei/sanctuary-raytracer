#pragma once
#include "Globals.h"

#include "CRTTypes.h"
#include "Scene.h"
#include "AnimationComponent.h"

namespace Scripts {
    void onSceneLoaded(Scene& scene) {
        GEndFrame = zzzzzzzzzzzzzzzzz

        std::vector<Keyframe<Vec3>> posKfs {
            {0.f, {0.f, 14.f, 26.f}},
            {2.f, {0.f, 14.f, 0.f}},
        };
        Matrix3x3 mat0 = {{
			1.f, 0.f, 0.f,
			0.f, 0.906307757f, -0.422618270f,
			0.f, 0.422618270f, 0.906307757f
            }};

        Vec3 dir = { -1.f, -0.4f, 0.f };
        dir.normalize();
        Vec3 right = Matrix3x3::Pan(-90.f) * dir;
        Vec3 up = Matrix3x3::Tilt(90.f) * dir;

        Matrix3x3 mat1 = Matrix3x3::fromCols(right, up, dir);

        std::vector<Keyframe<Matrix3x3>> matKfs {
            {0.f, mat0},
            {4.f, mat1},
        };
        scene.cameraAnimations[0] = AnimationComponent{};
        scene.cameraAnimations[0].pos = AnimationCurve{posKfs};
        scene.cameraAnimations[0].mat = AnimationCurve{matKfs};


    }

} // namespace Scripts

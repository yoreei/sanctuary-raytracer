#pragma once

#include<string>

#include "Camera.h"
#include "Scene.h"
#include "Animation.h"
#include "Animator.h"

namespace UnitTestData {

    void loadCamera1(Camera& camera)
    {
        float fov = 90.f;
        Vec3 camPos = { 0.f,0.f,0.f };
        Vec3 dir = { 0.f, 0.f, -1.f };
        dir = dir.normalize();
        Vec3 right = Matrix3x3::Pan(-90.f) * dir;
        Vec3 up = Matrix3x3::Tilt(90.f) * dir;

        Matrix3x3 camMat = Matrix3x3::fromCols(right, up, dir);
        camera = Camera{ fov, camPos, camMat };
    }



    void loadScene1(Scene& scene, Animator& animator)
    {
        scene.vertices = {
        {-1.75f, -1.75f, -3.f},
        {1.75f, -1.75f, -3.f},
        {0.f, 1.75f, -3.f},
        };

        scene.triangles = {
            {0, 1, 2}
        };

        assert(scene.bakeObject());
        Camera camera{};
        loadCamera1(camera);
        scene.camera = camera;
    }

    void loadScene2(Scene& scene, Animator& animator)
    {
        Camera camera {};
        loadCamera1(camera);
        scene.camera = camera;

        scene.camera.animations.push_back(RotateAnimation::Make(RotateType::Pan, 10.f, 0, 1));
        scene.camera.animations.push_back(RotateAnimation::Make(RotateType::Roll, 10.f, 1, 2));
        scene.camera.animations.push_back(RotateAnimation::Make(RotateType::Tilt, 10.f, 2, 3));  
        scene.camera.animations.push_back(MoveAnimation::Make(MoveType::Dolly, 0.5f, 3, 4));
        scene.camera.animations.push_back(MoveAnimation::Make(MoveType::Pedestal, 0.5f, 4, 5));
        scene.camera.animations.push_back(MoveAnimation::Make(MoveType::Truck, 0.5f, 5, 6));

        std::vector<Triangle> task1 =
        { Triangle::fromVertices(
            {-1.75f, -1.75f, -3.f},
            {1.75f, -1.75f, -3.f},
            {0.f, 1.75f, -3.f}
            , scene.vertices) 
        };
        scene.triangles = task1;

        animator.endFrame = 7;
    }

    void load(const std::string& name, Scene& scene, Animator& animator)
    {
        if (name == "scene1")
        {
            loadScene1(scene, animator);
        }
        else if (name == "scene2")
        {
            loadScene2(scene, animator);
        }
        else
        {
            throw std::runtime_error("Unknown scene name: " + name);
        }
    }

} // namespace UnitTestData

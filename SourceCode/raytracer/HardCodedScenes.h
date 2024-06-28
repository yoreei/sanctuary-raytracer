#pragma once

namespace Scenes {

    void loadCamera1(Camera& camera)
    {
        float fov = 90.f;
        Vec3 camPos = { 0.f,0.f,0.f };
        Vec3 dir = { 0.f, 0.f, -1.f };
        dir = dir.normalize();
        Vec3 right = Matrix3x3::Pan(90.f) * dir;
        Vec3 up = Matrix3x3::Tilt(90.f) * dir;

        std::cout << "Cam: [ " << right << " " << up << " " << dir << " ] " << std::endl;

        Matrix3x3 camMat = Matrix3x3::fromCols(right, up, dir);
        camera = Camera{ fov, camPos, camMat };
    }

    void loadTask1(Scene& scene)
    {
        Camera camera {};
        loadCamera1(camera);
        scene.camera = camera;

        scene.camera.animations.push_back(RotateAnimation::Make(RotateType::Roll, 360.f, 0, 24));

        std::vector<Triangle> task1 =
        { {
            Vec3{-1.75f, -1.75f, -3.f},
            Vec3{1.75f, -1.75f, -3.f},
            Vec3{0.f, 1.75f, -3.f},
        } };
        scene.triangles = task1;
    }

    //// Triangles for hw5 task2
    //// all vertices even with the image plane
    //Triangle tri_even
    //{
    //    Vec3{-1.f, -1.f, -1.f},
    //    Vec3{1.f, -1.f, -1.f},
    //    Vec3{0.f, 1.f, -1.f},
    //};


    //// Bottom vert deeper
    //Triangle tri1
    //{
    //    Vec3{-1.f, -1.f, -1.f},
    //    Vec3{-0.8f, -1.f, -1.f},
    //    Vec3{-0.9f, -0.8f, -1.5f}
    //};

    //// right vert deeper
    //Triangle tri2
    //{
    //    Vec3{-1.f, -0.8f, -1.f},
    //    Vec3{-1.f, -1.f, -1.f},
    //    Vec3{-0.8f, -0.9f, -1.5f},
    //};

    //// top vert deeper
    //Triangle tri3
    //{
    //    Vec3{0.8f,1.f,-1.f},
    //    Vec3{0.9f,0.8f,-1.5f},
    //    Vec3{1.f,1.f,-1.f},
    //};

    //// Task 3
    //std::vector<Triangle> task3 = {
    //    {
    //        Vec3{-1.f, -1.f, -1.f},
    //        Vec3{1.f, -1.f, -1.f},
    //        Vec3{-1.f, 1.f, -1.f},
    //    },
    //    {
    //        Vec3{1.f, -1.f, -1.f},
    //        Vec3{1.f, 1.f, -1.f},
    //        Vec3{-1.f, 1.f, -1.f},
    //    }
    //};

    // Task 4
    std::vector<Triangle> foxShape = {
        {
            Vec3{-1.f, -1.f, -2.f},
            Vec3{1.f, -1.f, -2.f},
            Vec3{0.f, 1.f, -2.3f},
        },
        {
            Vec3{-1.f, -1.f, -2.f},
            Vec3{-0.9f, -1.3f, -2.1f},
            Vec3{-0.8f, -1.f, -2.f},
        },
        {
            Vec3{0.8f, -1.f, -2.f},
            Vec3{0.9f, -1.3f, -2.1f},
            Vec3{1.f, -1.f, -2.f},
        },
    };

    void loadHw6task3and4(Scene& scene, Animator& animator)
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
        { {
            Vec3{-1.75f, -1.75f, -3.f},
            Vec3{1.75f, -1.75f, -3.f},
            Vec3{0.f, 1.75f, -3.f},
        } };
        scene.triangles = task1;

        animator.endFrame = 7;
    }
} // namespace Scenes

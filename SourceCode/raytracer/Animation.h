#pragma once
#include "CRTTypes.h"

struct SceneObject;

class Animation
{
public:
    virtual void apply(SceneObject& obj) = 0;
protected:
    int frames = 0;
    CURRFRAME
    Animation(int frames) : frames(frames) {}
};

class MoveAnimation : public Animation 
{
public:
    MoveAnimation(Vec3 delta, int frames) : Animation(frames), delta(delta) {}
    virtual void apply(SceneObject& obj) override;

private:
    Vec3 delta {0.f, 0.f, 0.f};
};

class RotateAnimation : public Animation
{
public:
    RotateAnimation(Matrix3x3 mat, int frames) : Animation(frames), mat(mat) {}
    virtual void apply(SceneObject& obj) override;

private:
    Matrix3x3 mat;
};
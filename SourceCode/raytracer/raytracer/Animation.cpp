#pragma warning( disable : 4365 )

#include "Animation.h"
#include "SceneObject.h"


void MoveAnimation::apply(SceneObject& obj)
{
    // TODO: cache animDir for performance?
    Vec3 animDir = obj.mat * delta;
    obj.pos = obj.pos + animDir;
}

std::string MoveAnimation::toString() const
{
    return "MoveAnimation { delta: " + delta.toString();
}

void RotateAnimation::apply(SceneObject& obj)
{
    obj.mat = mat * obj.mat;
}

std::string RotateAnimation::toString() const
{
    return "RotateAnimation { mat: " + mat.toString();
}

#include "Animation.h"
#include "SceneObject.h"


void MoveAnimation::apply(SceneObject& obj)
{
    // TODO: cache animDir for performance?
    Vec3 animDir = obj.mat * delta;
    obj.pos = obj.pos + animDir;
}

void RotateAnimation::apply(SceneObject& obj)
{
    obj.mat = mat * obj.mat;
}

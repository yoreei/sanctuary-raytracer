#include "Animation.h"
#include "SceneObject.h"

void MoveAnimation::apply(SceneObject& obj)
{
    obj.pos = obj.pos + delta;
}

void RotateAnimation::apply(SceneObject& obj)
{
    obj.mat = mat * obj.mat;
}
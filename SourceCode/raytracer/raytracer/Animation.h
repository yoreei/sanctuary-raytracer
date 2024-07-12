#pragma once
#include "CRTTypes.h"
#include <memory>

class SceneObject;

class Animation
{
public:
    virtual void apply(SceneObject& obj) = 0;
    auto getStartFrame() const { return startFrame; }
    auto getEndFrame() const { return endFrame; }
    virtual std::string toString() const = 0;
    virtual ~Animation() = default;

protected:
    int startFrame = 0;
    int endFrame = 0;
    Animation(int startFrame, int endFrame) : startFrame(startFrame), endFrame(endFrame) {}
};

enum class MoveType
{
    Dolly,
    Pedestal,
    Truck
};

class MoveAnimation : public Animation 
{

public:
    /*
    * Will apply `delta` on every frame
    */
    MoveAnimation(const Vec3 delta, int startFrame, int endFrame) : Animation(startFrame, endFrame), delta(delta) {}

    /*
    * Will distribute movement over `endFrame - startFrame` frames
    */
    static std::shared_ptr<Animation> Make(MoveType type, float distance, int startFrame, int endFrame)
    {
        Vec3 _delta;
        switch (type)
        {
        case MoveType::Dolly:
            _delta = { 0.f, 0.f, distance / static_cast<float>(endFrame - startFrame) };
            break;
        case MoveType::Pedestal:
            _delta = { 0.f, distance / static_cast<float>(endFrame - startFrame), 0.f };
            break;
        case MoveType::Truck:
            _delta = { distance / static_cast<float>(endFrame - startFrame), 0.f, 0.f };
            break;
        default:
            throw std::invalid_argument("Invalid MoveType");
        }
        return std::make_shared<MoveAnimation>(_delta, startFrame, endFrame);
    }

    virtual void apply(SceneObject& obj) override;
    virtual std::string toString() const override;

private:
    Vec3 delta = { 0.f, 0.f, 0.f };
    char padding[4]; // Padding to make sure the size of the class is a multiple of 16 bytes
    };

enum class RotateType
{
    Pan,
    Tilt,
    Roll
};

class RotateAnimation : public Animation
{

public:
    /*
    * Will apply `mat` on every frame
    */
    RotateAnimation(Matrix3x3 mat, int startFrame, int endFrame) : Animation(startFrame, endFrame), mat(mat) {}

    /*
    * Will distribute rotation over `endFrame - startFrame` frames
    */
    static std::shared_ptr<Animation> Make(RotateType type, float angle, int startFrame, int endFrame)
    {
        Matrix3x3 _mat;
        switch (type)
        {
        case RotateType::Pan:
            _mat = Matrix3x3::Pan(angle / static_cast<float>(endFrame - startFrame));
            break;
        case RotateType::Tilt:
           _mat = Matrix3x3::Tilt(angle / static_cast<float>(endFrame - startFrame));
           break;
        case RotateType::Roll:
           _mat = Matrix3x3::Roll(angle / static_cast<float>(endFrame - startFrame));
           break;
        default:
            throw std::invalid_argument("Invalid MoveType");
        }

        return std::make_shared<RotateAnimation>(_mat, startFrame, endFrame);
    }

    virtual void apply(SceneObject& obj) override;
    virtual std::string toString() const override;

private:
    Matrix3x3 mat;
    char padding[4]; // Padding to make sure the size of the class is a multiple of 16 bytes
};

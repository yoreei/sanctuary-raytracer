#include "CRTTypes.h"
#include "Globals.h"
#include "json.hpp"

bool Ray::compareRefract(const Vec3 point, const Vec3& N, float etai, float etat)
{
    Vec3 oldDirection = direction;
    Vec3 oldOrigin = origin;
    refractSP(point, N, etai, etat);
    Vec3 spDirection = direction;

    bool result = refractVladi(point, N, etai, etat);
    static Vec3 fError = spDirection - direction;
    if (fError.lengthSquared() > GlobalDebug::maxError.lengthSquared()) {
        GlobalDebug::maxError = fError;
        GlobalDebug::maxErrorDirection = oldDirection;
    }
    fError = { fabs(fError.x), fabs(fError.y), fabs(fError.z) };
    GlobalDebug::fErrors += fError;

    return result;
}


bool Ray::refractSP(const Vec3 point, const Vec3& N, float etai, float etat)
{
    float cosi = dot(direction, N);
    assert(cosi < 1e-6);
    cosi = std::clamp(cosi, -1.f, 0.f);
    float eta = etai / etat;

    float cosTSq = 1.f - eta * eta * (1.f - cosi * cosi);
    if (cosTSq < 0.f) return false; // Total Internal Reflection
    float cosT = sqrtf(cosTSq);
    cosT = std::clamp(cosT, -1.f, 1.f);
    direction = eta * direction + (eta * cosi - cosT) * N;
    direction.normalize();
    // explanation:
    // T = X + Y
    // X = -N * cosT
    // Y = eta * I + eta * cosi * N 
    // = eta * (I + cosI * N)
    // = M * sinT 
    // M = (I + cosI * N) * 1 / sinI, where M is unit surface tangent in plane of incidence
    // sinT = eta * sinI
    origin = point;

#ifndef NDEBUG
    float cosTTest = fabs(dot(direction, N));
    cosTTest = std::clamp(cosTTest, -1.f, 1.f);
    assert(fEqual(cosT, cosTTest, 0.1f)); // very low precision here.
#endif
    return true;
}

/**
* @brief: Inuitive implementation, suffers from precision issues.
* @return false if no refraction (i.e. total internal reflection)
* @param normal: expected to face the ray.
**/

bool Ray::refractVladi(const Vec3& point, Vec3 normal, float iorI, float iorR) {
#ifndef NDEBUG
    auto dbgOldDirection = direction;
    assert(fEqual(direction.lengthSquared(), 1.f));
    assert(dot(direction, normal) < -epsilon); // usage req: normal should face the ray
#endif

    float cosI = -dot(direction, normal);
    float sinR = sqrt(1 - cosI * cosI) * iorI / iorR;
    if (sinR - 1.f >= -1e-6f) {
        // total internal reflection
        return false;
    }
    float cosR = sqrt(1 - sinR * sinR);
    // bump direction to avoid 0 vector check // TODO ask in Discord
    direction = direction * (1.f + 1e-6f);
    Vec3 C = (direction + cosI * normal).getUnit();
    Vec3 B = C * sinR;
    Vec3 A = -normal * cosR;
    direction = A + B;
    direction.normalize(); // TODO ask in Discord: can we avoid this?
    origin = point;

    assert(fEqual(dot(direction, normal), -cosR, 1e-2f)); // very low precision here. This one fails for hw11/scene8
    // assert R not going backwards:
    assert(dot(direction, dbgOldDirection) > 0);
    return true;
}

nlohmann::ordered_json Vec3::toJson() const {
    nlohmann::ordered_json j{};
    j["x"] = x;
    j["y"] = y;
    j["z"] = z;
    return j;
}

void Vec3::componentMax(const Vec3& vec0, const Vec3& vec1, Vec3& vec2Output)
{
    vec2Output.x = std::max({ vec0.x, vec1.x, vec2Output.x });
    vec2Output.y = std::max({ vec0.y, vec1.y, vec2Output.y });
    vec2Output.z = std::max({ vec0.z, vec1.z, vec2Output.z });
}

void Vec3::componentMin(const Vec3& vec0, const Vec3& vec1, Vec3& vec2Output)
{
    vec2Output.x = std::min({ vec0.x, vec1.x, vec2Output.x });
    vec2Output.y = std::min({ vec0.y, vec1.y, vec2Output.y });
    vec2Output.z = std::min({ vec0.z, vec1.z, vec2Output.z });
}

nlohmann::ordered_json Matrix3x3::toJson() const {
    nlohmann::ordered_json j;
    j["row0"] = row(0).toJson();
    j["row1"] = row(1).toJson();
    j["row2"] = row(2).toJson();
    return j;
}


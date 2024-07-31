#pragma once
#include <string>
#include "include/CRTTypes.h"
#include "include/Metrics.h"

namespace GlobalDebug
{
    extern Vec3 fErrors;
    extern Vec3 maxError;
    extern Vec3 maxErrorDirection;

    std::string toString();
}

// Only Engine should write to GFrameNumber
extern uint64_t GFrameNumber;
extern uint64_t GEndFrame;

extern void GResetGlobals();

extern Metrics GSceneMetrics;

extern uint64_t GBestTriangleIntersect;
extern std::string GBestSettings;

// Modified by Renderer worker threads on thread boot
extern thread_local size_t GThreadIdx;

static constexpr float PI = static_cast<float>(std::numbers::pi);

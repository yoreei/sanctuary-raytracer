#pragma once
#include <string>

#include "include/CRTTypes.h"
#include "include/Metrics.h"

// Only Engine should write to GFrameNumber
extern uint64_t GFrameNumber;
// Scripts are expected to set GEndFrame
extern uint64_t GEndFrame;

// Reset all global variables to their default values
extern void GResetGlobals();

extern Metrics GSceneMetrics;

extern uint64_t GBestTriangleIntersect;
extern std::string GBestSettings;

// Modified by Renderer worker threads on thread boot
extern thread_local size_t GThreadIdx;

static constexpr float PI = static_cast<float>(std::numbers::pi);

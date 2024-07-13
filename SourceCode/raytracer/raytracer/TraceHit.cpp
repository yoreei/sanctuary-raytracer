#include "TraceHit.h"

std::string toString(TraceHitType intersection) {
    switch (intersection) {
    case TraceHitType::SUCCESS: return "SUCCESS";
    case TraceHitType::INSIDE_REFRACTIVE: return "INSIDE_REFRACTIVE";
    case TraceHitType::OUT_OF_BOUNDS: return "OUT_OF_BOUNDS";
    case TraceHitType::PLANE_BEHIND_RAY_ORIGIN: return "PLANE_BEHIND_RAY_ORIGIN";
    case TraceHitType::PLANE_BACKFACE: return "PLANE_BACKFACE";
    case TraceHitType::PARALLEL: return "PARALLEL";
    default: return "UNKNOWN";
    }
}

#include "Intersection.h"

std::string toString(IntersectionType intersection) {
    switch (intersection) {
    case IntersectionType::SUCCESS: return "SUCCESS";
    case IntersectionType::PARALLEL: return "PARALLEL";
    case IntersectionType::BACKFACE: return "BACKFACE";
    case IntersectionType::OUT_OF_BOUNDS: return "OUT_OF_BOUNDS";
    case IntersectionType::BEHIND_RAY_ORIGIN: return "BEHIND_RAY_ORIGIN";
    default: return "UNKNOWN";
    }
}

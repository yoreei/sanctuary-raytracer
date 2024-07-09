#include "Intersection.h"

std::string toString(IntersectionType intersection) {
    switch (intersection) {
    case IntersectionType::SUCCESS: return "SUCCESS";
    case IntersectionType::INSIDE_REFRACTIVE: return "INSIDE_REFRACTIVE";
    case IntersectionType::OUT_OF_BOUNDS: return "OUT_OF_BOUNDS";
    case IntersectionType::PLANE_BEHIND_RAY_ORIGIN: return "PLANE_BEHIND_RAY_ORIGIN";
    case IntersectionType::PLANE_BACKFACE: return "PLANE_BACKFACE";
    default: return "UNKNOWN";
    }
}

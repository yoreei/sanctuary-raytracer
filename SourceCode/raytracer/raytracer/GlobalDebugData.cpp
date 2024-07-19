#include <string>
#include <sstream>
#include "GlobalDebugData.h"

namespace GlobalDebug {

    Vec3 fErrors {0.f, 0.f, 0.f};
    Vec3 maxError = {0.f, 0.f, 0.f};
    Vec3 maxErrorDirection = {0.f, 0.f, 0.f};

    std::string toString()
    {
        std::stringstream sstream {};
        sstream << "Error: " << fErrors << std::endl;
        sstream << "Max Error: " << maxError << std::endl;
        sstream << "Max Error Direction: " << maxErrorDirection << std::endl;
        return sstream.str();
    }

} // namespace GlobalDebug

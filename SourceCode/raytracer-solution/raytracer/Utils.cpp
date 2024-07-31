#include "include/Utils.h"
#include <string>
#include "json.hpp"

#include "include/CRTTypes.h"

namespace Utils {
    /* Only works with basic types */
    template <typename T>
    T jsonGetDefault(const nlohmann::json& j, const std::string& key, T defaultVal) {
        if (!j.contains(key)) {
            return defaultVal;
        }
        return j.at(key).get<T>();
    }

    template bool jsonGetDefault(const nlohmann::json& j, const std::string& key, bool defaultVal);
    template float jsonGetDefault(const nlohmann::json& j, const std::string& key, float defaultVal);
    template unsigned __int64 jsonGetDefault(const nlohmann::json& j, const std::string& key, unsigned __int64 defaultVal);
    template std::string jsonGetDefault(const nlohmann::json& j, const std::string& key, std::string defaultVal);

    template <>
    Vec3 jsonGetDefault(const nlohmann::json& j, const std::string& key, Vec3 defaultVal) {
        if (!j.contains(key)) {
            return defaultVal;
        }

        const auto& jVec = j.at(key);
        return { jVec[0], jVec[1], jVec[2] };

    }

} // namespace Utils


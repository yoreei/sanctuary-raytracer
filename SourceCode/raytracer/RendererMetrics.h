#pragma once

#include <string>
#include "json.hpp"
#include "Intersection.h"

using ordered_json = nlohmann::ordered_json;

class RendererMetrics {
public:

    void startTimer() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    void stopTimer() {
        endTime = std::chrono::high_resolution_clock::now();
        duration = endTime - startTime;
    }

    void record(Intersection x) {
        xCounts[x]++;
    }

    ordered_json toJson() const {
        ordered_json j;
        j["timer"] = duration.count();

        for (const auto& pair : xCounts) {
            j[::toString(pair.first)] = pair.second;
        }
        return j;
    }

    std::string toString() const {
        return toJson().dump(4);
    }

private:
    std::unordered_map<Intersection, int> xCounts;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    std::chrono::duration<double> duration = std::chrono::duration<double>::zero();
};

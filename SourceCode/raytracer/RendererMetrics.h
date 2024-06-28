#pragma once

#include <string>
#include "json.hpp"
#include "Intersection.h"

using ordered_json = nlohmann::ordered_json;

/* Helps for debugging the Renderer */
class RendererMetrics {
public:

    void startTimer() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    void stopTimer() {
        endTime = std::chrono::high_resolution_clock::now();
        duration = endTime - startTime;
    }

    void record(std::string s) {
        xCounts[s]++;
    }

    ordered_json toJson() const {
        ordered_json j;
        j["timer"] = duration.count();

        for (const auto& pair : xCounts) {
            j[(pair.first)] = pair.second;
        }
        return j;
    }

    std::string toString() const {
        return toJson().dump(4);
    }

private:
    std::unordered_map<std::string, int> xCounts;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    std::chrono::duration<double> duration = std::chrono::duration<double>::zero();
};

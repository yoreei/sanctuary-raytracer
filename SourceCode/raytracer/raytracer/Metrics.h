#pragma once

#include <string>
#include <chrono>
#include "json.hpp"
#include "TraceHit.h"

using ordered_json = nlohmann::ordered_json;

class Timer {
public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    void stop() {
       endTime = std::chrono::high_resolution_clock::now();
       duration += endTime - startTime;
    }
    std::chrono::duration<double> duration = std::chrono::duration<double>::zero();
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime {};
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime {};
};

/* Helps for debugging the Renderer */
class Metrics {
public:
    Metrics() = default;
    Metrics(const std::string& name) : name(name) {}
    void startTimer(const char* timerKey) {
        timers[timerKey].start();
    }

    void stopTimer(const char* timerKey) {
        timers[timerKey].stop();
    }

    void record(std::string s) {
        xCounts[s]++;
    }

    ordered_json toJson() const {
        ordered_json j;
        j["scene_name"] = name;
        j["timers"] = {};
        for (const auto& pair: timers) {
            const std::string& timerName = pair.first;
            const Timer& timer = pair.second;
            j["timers"][timerName] = timer.duration.count();
        }
        j["counters"] = {};
        for (const auto& pair : xCounts) {
            const std::string& counterName = pair.first;
            const int& counterValue = pair.second;
            j["counters"][counterName] = counterValue;
        }
        return j;
    }

    std::string toString() const {
        return toJson().dump(4);
    }

private:
    std::unordered_map<std::string, int> xCounts;
    std::unordered_map<std::string, Timer> timers {};
    std::string name {};
};

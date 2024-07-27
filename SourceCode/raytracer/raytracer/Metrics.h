#pragma once

#include <string>
#include <chrono>
#include "json.hpp"
#include "TraceHit.h"

using ordered_json = nlohmann::ordered_json;

using Counters = std::unordered_map<std::string, int>;

class PerThreadMetrics {
public:
     Counters xCounts;
};

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

    /* thread safe */
    void record(std::string s, std::thread::id threadId = std::this_thread::get_id()) {
        // We perform double-check locking to ensure lock-free metrics recording after the initial wave

        if (threads.find(threadId) == threads.end()) { // Fast Path
            std::lock_guard<std::mutex> lock(recordMutex); // Slow path

            if (threads.find(threadId) == threads.end()) {
                threads[threadId] = PerThreadMetrics();
            }
        }
        
        threads[threadId].xCounts[s]++;
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

        auto summedCounters = Counters{};
        for (const auto& [_, thread] : threads) {
            _;
            for (auto& [key, val] : thread.xCounts) {
                summedCounters[key] += val;
            }
        }

        j["counters"] = {};
        for (const auto& [key, val] : summedCounters) {
            j["counters"][key] = val;
        }
        return j;
    }

    std::string toString() const {
        return toJson().dump(4);
    }

    void clear() {
        threads.clear();
        timers.clear();
        name.clear();
    }

private:
    std::unordered_map<std::thread::id, PerThreadMetrics> threads {};
    std::unordered_map<std::string, Timer> timers {};
    std::mutex recordMutex;
    std::string name {};
};

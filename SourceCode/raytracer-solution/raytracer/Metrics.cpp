#include "include/Metrics.h"

#include "json.hpp"

#include "include/Globals.h"

void Metrics::startTimer(const char* timerKey) {
    timers[timerKey].start();
}

void Metrics::stopTimer(const char* timerKey) {
    timers[timerKey].stop();
}

void Metrics::record(std::string s) {
    threads[GThreadIdx].xCounts[s]++;
}

ordered_json Metrics::toJson() const {
    ordered_json j;
    j["scene_name"] = name;
    j["timers"] = {};
    for (const auto& pair : timers) {
        const std::string& timerName = pair.first;
        const Timer& timer = pair.second;
        j["timers"][timerName] = timer.duration.count();
    }

    auto summedCounters = Counters{};
    for (const PerThreadMetrics& threadMetrics : threads) {
        for (auto& [key, val] : threadMetrics.xCounts) {
            summedCounters[key] += val;
        }
    }

    j["counters"] = {};
    for (const auto& [key, val] : summedCounters) {
        j["counters"][key] = val;
    }
    return j;
}

std::string Metrics::toString() const {
    return toJson().dump(4);
}

void Metrics::clear() {
    threads.clear();
    timers.clear();
    name.clear();
}

void Metrics::reserveThread(size_t numThreads) {
    threads.resize(numThreads, PerThreadMetrics{});
}

void Timer::start() {
    startTime = std::chrono::high_resolution_clock::now();
}

void Timer::stop() {
    endTime = std::chrono::high_resolution_clock::now();
    duration += endTime - startTime;
}

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include "json_fwd.h"

using ordered_json = nlohmann::ordered_json;

using Counters = std::unordered_map<std::string, int>;

class PerThreadMetrics {
public:
     Counters xCounts;
};

class Timer {
public:
    void start();
    void stop();
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
    void startTimer(const char* timerKey);

    void stopTimer(const char* timerKey);

    /* thread-safe */
    void record(std::string s);

    ordered_json toJson() const;

    std::string toString() const;

    void clear();

    // non-thread safe, must call before launching threads
    void reserveThread(size_t numThreads);

private:
    std::vector<PerThreadMetrics> threads {};
    std::unordered_map<std::string, Timer> timers {};
    std::mutex reserveThreadMutex;
    std::string name {};
};

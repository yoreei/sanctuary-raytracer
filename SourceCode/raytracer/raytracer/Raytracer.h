#pragma once
#include <string>
#include <vector>
#include "Settings.h"

class Metrics;
class Raytracer {
public:
    Raytracer(const Settings& settings) : settings(settings) {}
    void writeFile(const std::string& filename, const std::string& data);
    void runScene(const std::string& sceneName, Metrics& metrics);
    int run();

private:
    void writeMetrics(const std::vector<Metrics>& metricsList);
    const Settings& settings;
};
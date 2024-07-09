#pragma once
#include <string>
#include <vector>

class Metrics;
class Raytracer {
public:
    static void writeFile(const std::string& filename, const std::string& data);
    static void runScene(const std::string& sceneName, Metrics& metrics);
    static int run();

private:
    static void writeMetrics(const std::vector<Metrics>& metricsList);
    static const std::string INPUT_DIR;
};
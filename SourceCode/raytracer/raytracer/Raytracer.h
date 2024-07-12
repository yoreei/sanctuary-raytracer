#pragma once
#include <string>

class Raytracer {
public:

static void writeFile(const std::string& filename, const std::string& data);

static void runScene(const std::string& sceneName);

static int run();

};
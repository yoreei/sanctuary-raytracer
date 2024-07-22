#include <cassert>

#include "Raytracer.h"
#include "Settings.h"

int main() {

    Settings settings = Settings::load("settings.json");
    Raytracer rt {settings};
    rt.run();

    return 0;

}
#include <cassert>

#include "Raytracer.h"
#include "Settings.h"

int main() {

    Settings settings {
        .inputDir = "scenes/in",
        .debugPixel = false,
        .debugPixelX = 1300,
        .debugPixelY = 200
    };
    Raytracer rt {settings};
    rt.run();

    return 0;

}
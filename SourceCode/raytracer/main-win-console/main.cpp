#pragma warning( disable : 4365 )

// Enable debug heap
//#ifdef _DEBUG
//#include <crtdbg.h>
//#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#endif

#include "Raytracer.h"
#include "Settings.h"

int main() {
//#ifdef _DEBUG
//    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif

    Settings settings {.inputDir = "scenes/in"};
    Raytracer rt {settings};
    rt.run();
    //Raytracer::runScene("scene1");

//#ifdef _DEBUG
//    _CrtDumpMemoryLeaks();
//#endif

    return 0;

}
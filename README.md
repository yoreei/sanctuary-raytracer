# Danny Grigorov's Chaos Camp 2024 Homeworks and Final Project

## Final Project

https://www.youtube.com/watch?v=l6pGnYhn9KA

A simple 3D ray-caster supporting reflection & refraction. My original work for Chaos Camp 2024 ray tracing course. Participating in the Chaos Camp was an incredible experience, and I am extremely grateful to the teaching staff and the other participants for making this possible. This was my first time diving into computer graphics, and I can confidently say that I am totally hooked now.

The flickering foot of the crystal dragon you see on the video is fixed in main.

## Repository Layout

- `SourceCode/raytracer-solution/` - the ray tracer (Visual Studio solution `solution.sln`)
  - `raytracer/` - the rendering library
  - `main-win-console/` - console front end; scenes live in `sceneLibrary/`, renders go to `out/`
  - `tests/` - unit tests
  - `third-party/` - vendored single-header libraries (nlohmann/json, stb_image, perlin-noise)
  - `blender_projects/` - Blender file used to prepare the skyboxes
- `SourceCode/hw1-CUDA-draw-rectangle/`, `SourceCode/hw2-CUDA-draw-circle/` - early CUDA homeworks
- `Images/` - homework render results

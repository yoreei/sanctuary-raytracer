#pragma once

class Scene;
namespace Scripts {

    /* @brief Called by Engine before the first tick */
    void onSceneLoaded(Scene& scene);
    void onTick(Scene& scene);
} // namespace Scripts

#ifndef N64_WORLD_H
#define N64_WORLD_H

#include "Scene.h"
#include "SceneId.h"
#include <cstdint>
#include <memory>
#include <libdragon.h>

namespace n64
{
    constexpr SceneId INVALID_SCENE_ID = -1;

    class World
    {
    public:
        World() : mScene(new Scene()), mNextSceneId(INVALID_SCENE_ID) {}
        ~World() = default;
        World(const World &other) = delete;
        World &operator=(const World &other) = delete;
        World(World &&other) = delete;
        World &operator=(World &&other) = delete;

        SceneId nextSceneId() const { return mNextSceneId; }

        void fixedloop(float deltatime);
        void loop(float deltatime);

        // Use mNextSceneId to locate and load the scene
        void loadNextScene() { debugf("World::loadNextScene - Not implemented"); };

        // Free all the memory associated with the current scene
        void unloadCurrentScene() { debugf("World::unloadCurrentScene - Not implemented"); };

    private:
        std::unique_ptr<Scene> mScene;
        SceneId mNextSceneId;
    };
}

#endif // N64_WORLD_H
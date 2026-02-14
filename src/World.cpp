#include "World.h"

namespace n64
{
    void World::fixedloop(float deltatime)
    {
        mScene->updateFixed(deltatime);
    }

    void World::loop(float deltatime)
    {
        mScene->update(deltatime);
    }
}
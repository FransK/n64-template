#pragma once

#include "scene/scene.h"

namespace Fishing
{
    class World
    {
    public:
        World();
        ~World();
        World(const World &other) = delete;
        World &operator=(const World &other) = delete;

        void fixedloop(float deltatime);
        void loop(float deltatime);

    private:
        Scene *scene;
    };
}
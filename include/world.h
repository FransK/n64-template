#pragma once

#include "scene/scene.h"
#include <memory>

class World
{
public:
    World();
    ~World();
    World(const World &other) = delete;
    World &operator=(const World &other) = delete;

    void fixedloop(float deltatime);
    void loop(float deltatime);
    void reset();

private:
    std::unique_ptr<Scene> scene;
};
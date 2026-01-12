#pragma once

#include "scene/scene.h"

namespace Fishing
{
    extern Scene *scene;

    void init();
    void fixedloop(float deltatime);
    void loop(float deltatime);
    void cleanup();
}
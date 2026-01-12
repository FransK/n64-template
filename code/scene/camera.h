#pragma once

#include <t3d/t3d.h>
#include <t3d/t3dmath.h>

struct Camera
{
    T3DVec3 position{};
    T3DVec3 target{};

    void update(T3DViewport &viewport);
};
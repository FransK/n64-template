#ifndef PLAYER_POSITION_H
#define PLAYER_POSITION_H

#include "../math/vector2.h"
#include "../math/vector3.h"

using namespace Math;

struct PlayerData
{
    Vector3 position{};
    Vector2 rotation{};
    Vector3 velocity{};
};

#endif
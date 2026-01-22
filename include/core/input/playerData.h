#ifndef PLAYER_POSITION_H
#define PLAYER_POSITION_H

#include "math/vector2.h"
#include "math/vector3.h"
#include "scene/actor.h"

using namespace Math;

class PlayerData : public Actor
{
public:
    PlayerData() : Actor() {}

    [[nodiscard]] Actor *getAttackActor() { return &mAttackActor; }

private:
    Actor mAttackActor{};
};

#endif
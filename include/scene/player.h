#pragma once

#include <cstdint>
#include <t3d/t3danim.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include "animation/animationComponent.h"
#include "collision/CollisionScene.h"
#include "collision/collider.h"
#include "collision/cylinder.h"
#include "collision/sphere.h"
#include "input/inputState.h"
#include "input/playerData.h"
#include "input/playerState.h"
#include "math/vector2.h"
#include "playerColliders.h"

using namespace Math;

class Player
{
private:
    Collision::Collider mCollider{};
    Collision::Collider mDamageTrigger{};
    int mEntityId{-1};

    Collision::CollisionScene *mScene{};
    PlayerData *mPlayerData{};
    PlayerState *mPlayerState{};

public:
    void init(Collision::CollisionScene *scene, PlayerData *data, PlayerState *state, int8_t playerNumber);
    void draw_billboard(T3DViewport &viewport) const;

    Collision::Collider *get_collider() { return &mCollider; }
    Collision::Collider *get_damage_trigger() { return &mDamageTrigger; }

    Player() = default;
    ~Player();
};
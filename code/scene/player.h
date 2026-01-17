#pragma once

#include <cstdint>
#include <t3d/t3danim.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include "../animation/animationComponent.h"
#include "../collision/scene.h"
#include "../collision/collider.h"
#include "../collision/cylinder.h"
#include "../input/inputState.h"
#include "../math/vector2.h"
#include "../player/playerData.h"
#include "../player/playerState.h"

using namespace Math;

namespace Fishing
{
    constexpr float BILLBOARD_YOFFSET = 200.f;

    constexpr float HITBOX_RADIUS = 5.f;
    constexpr float ATTACK_OFFSET = 5.f;
    constexpr float ATTACK_RADIUS = 10.f;

    constexpr float SHOVE_DIST = 7.f;

    constexpr int8_t FIRST_PLAYER_COLLIDER_GROUP = 1;

    constexpr Collision::ColliderType PlayerColliderType = {
        .minkowskiSum = Collision::Cylinder::MinkowskiSum,
        .boundingBoxCalculator = Collision::Cylinder::BoundingBox,
        .data = {
            .cylinder = {
                .radius = 5.0f,
                .halfHeight = 12.0f}},
        .bounce = 0.0f,
        .friction = 0.0f};

    class Player
    {
    private:
        Collision::Collider mCollider{};
        int mEntityId{-1};

        Collision::Scene *mScene{};
        PlayerData *mPlayerData{};
        PlayerState *mPlayerState{};

    public:
        void init(Collision::Scene *scene, PlayerData *data, PlayerState *state, int8_t playerNumber);
        void draw_billboard(T3DViewport &viewport) const;

        void get_attack_position(Vector2 &attack_pos) const;

        Player() = default;
        ~Player();
    };
}
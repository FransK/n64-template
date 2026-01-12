#pragma once

#include <cstdint>
#include <t3d/t3danim.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include "../collision/scene.h"
#include "../collision/collider.h"
#include "../collision/cylinder.h"
#include "../input/inputstate.h"
#include "../math/vector2.h"

using namespace Math;

namespace Fishing
{
    constexpr float PLAYER_SCALE = 0.125f;

    constexpr float BILLBOARD_YOFFSET = 200.f;

    constexpr float HITBOX_RADIUS = 5.f;
    constexpr float ATTACK_OFFSET = 5.f;
    constexpr float ATTACK_RADIUS = 10.f;

    constexpr float BASE_SPEED = 70.f;
    constexpr float SHOVE_DIST = 7.f;
    constexpr float MIN_MOVE_INPUT = 8.f;
    constexpr float ROTATION_INPUT = 26.f;

    constexpr int8_t FIRST_PLAYER_COLLIDER_GROUP = 1;

    constexpr float CATCH_TIMER = 1.f;

    constexpr float SHOVE_TIME_SCALE = 2.0f;
    constexpr float SHOVE_TIME = 21.0f / (30.0f * SHOVE_TIME_SCALE);
    constexpr float RECEIVE_SHOVE_TIME = SHOVE_TIME * 3.0f;
    constexpr float CAST_TIME = 21.0f / 30.0f;

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
        enum Anim : uint8_t
        {
            IDLE = 0,
            RUN,
            SHOVE,
            RECEIVE_SHOVE,
            CAST
        };

        Collision::Scene *mScene{};

        T3DModel *mModel{};
        T3DMat4FP *mModelMatFP{};
        rspq_block_t *mDplPlayer{};

        Collision::Collider mCollider{};

        T3DAnim mAnimIdle{};
        T3DAnim mAnimPunch{};
        T3DAnim mAnimReceiveHit{};
        T3DAnim mAnimRun{};
        T3DAnim mAnimCast{};
        T3DAnim *mActiveAnim{};
        T3DSkeleton mSkeleton{};

        float mSpeed{BASE_SPEED};
        float mFishingTimer{};
        float mAnimTimer{};
        float mCastTimer{};
        int8_t mFishCaught{};
        int8_t mPlayerNumber{-1};
        color_t mColor{};

        void play_animation(Anim anim);
        void update_animation(float deltaTime);
        void cancel_actions();

    public:
        [[nodiscard]] constexpr bool is_casting() const { return mCastTimer > 0.0f; }
        [[nodiscard]] constexpr bool is_fishing() const { return mFishingTimer > 0.0f; }
        [[nodiscard]] constexpr bool is_catchable() const { return mFishingTimer < CATCH_TIMER && mFishingTimer > 0.0f; }
        [[nodiscard]] constexpr bool can_move() const { return mFishingTimer <= 0.0f && mAnimTimer <= 0.0f; }

        void init(int8_t playerNumber, T3DVec3 position, Vector2 rotation, color_t color);
        void update_fixed(float deltaTime, InputState input);
        void update(float deltaTime, InputState input);
        uint32_t draw(T3DViewport &viewport, const T3DVec3 &cameraPos) const;
        void draw_billboard(T3DViewport &viewport, const T3DVec3 &cameraPos) const;

        void get_attack_position(Vector2 &attack_pos) const;
        [[nodiscard]] uint8_t get_fish_caught() const;
        const Vector3 &get_position() const;
        [[nodiscard]] Vector3 get_closest_fish() const;

        void shove();
        void receive_shove();
        void cast();

        Player(Collision::Scene *scene, T3DModel *model);
        ~Player();
    };
}
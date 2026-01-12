#pragma once

#include "../math/vector2.h"
#include "../math/vector3.h"
#include "player.h"

namespace Fishing
{
    enum AIBehavior : uint8_t
    {
        BEHAVE_FISHERMAN,
        BEHAVE_BULLY,
        BEHAVE_BALANCED,
    };

    class PlayerAi
    {
        enum AIState : uint8_t
        {
            STATE_IDLE,
            STATE_MOVE_TO_PLAYER,
            STATE_ATTACK,
            STATE_MOVE_TO_FISH,
            STATE_FISH,
            STATE_ANIMATION_LOCKED
        };

        Player mPlayer;
        AIState mCurrentState;
        AIState mNextState;
        AIBehavior mBehavior;
        InputState mInputState{};
        Player *mTarget{};
        Vector3 mMovementTarget{};
        float mSpeed{70.0f};
        float mAnimationLockedTimer{0.0f};
        float mDelayActionTimer{0.0f};
        float mDelayCatchTimer{0.0f};

        void change_state(AIState newState);
        void update_idle(float deltaTime);
        void update_move_to_player(float deltaTime);
        void update_attack(float deltaTime);
        void update_move_to_fish(float deltaTime);
        void update_fish(float deltaTime);
        void update_animation_locked(float deltaTime);
        void move_to_target();

    public:
        PlayerAi(Collision::Scene *scene, T3DModel *model, AIBehavior behavior);

        void init(int8_t playerNumber, T3DVec3 position, Vector2 rotation, color_t color);
        [[nodiscard]] Player *get_player();
        void receive_shove();
        void update_fixed(float deltaTime, Player *currentLeader);
        void update(float deltaTime);
    };
}
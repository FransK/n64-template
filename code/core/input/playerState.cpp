#include "playerState.h"

#include "animation/animationComponent.h"
#include "math/vector3.h"
#include "../../scene/playerColliders.h"

void PlayerState::changeState(const PlayerStateEnum &newState, PlayerData &playerData, Collision::CollisionScene &collScene, Collision::Collider *damageTrigger)
{
    if (mState == newState)
    {
        return;
    }

    if (mState == PlayerStateEnum::STATE_ATTACKING)
    {
        collScene.deactivate(damageTrigger);
    }

    if (mState == PlayerStateEnum::STATE_FISHING && mActionSuccess)
    {
        mFishCaught += 1;
        mActionSuccess = false;
    }

    mState = newState;

    switch (mState)
    {
    case PlayerStateEnum::STATE_ATTACKING:
        mStateTimer = SHOVE_TIME;
        playerData.setVelocity({0.0f, 0.0f, 0.0f});
        playerData.getAttackActor()->setPosition({playerData.getPosition().x + playerData.getRotation().x * ATTACK_OFFSET,
                                                  playerData.getPosition().y + PlayerColliderType.data.cylinder.halfHeight,
                                                  playerData.getPosition().z + -playerData.getRotation().y * ATTACK_OFFSET});
        collScene.activate(damageTrigger);
        notify();
        break;
    case PlayerStateEnum::STATE_CASTING:
        mStateTimer = CAST_TIME;
        playerData.setVelocity({0.0f, 0.0f, 0.0f});
        notify();
        break;
    case PlayerStateEnum::STATE_FISHING:
        // mStateTimer = Fish::get_new_timer();
        mStateTimer = 4.0f; // Temporary fixed timer
        playerData.setVelocity({0.0f, 0.0f, 0.0f});
        notify();
        break;
    case PlayerStateEnum::STATE_STUNNED:
        mStateTimer = RECEIVE_SHOVE_TIME;
        playerData.setVelocity({0.0f, 0.0f, 0.0f});
        notify();
        break;
    case PlayerStateEnum::STATE_IDLE:
        mStateTimer = 0.0f;
        playerData.setVelocity({0.0f, 0.0f, 0.0f});
        notify();
        break;
    case PlayerStateEnum::STATE_WALKING:
        mStateTimer = 0.0f;
        notify();
        break;
    }
}

void PlayerState::reset()
{
    mState = PlayerStateEnum::STATE_IDLE;
    mStateTimer = 0.0f;
    mActionSuccess = false;
    mFishCaught = 0;
}

void PlayerState::update(float deltaTime, PlayerData &playerData, Collision::CollisionScene &collScene, Collision::Collider *damageTrigger, bool stunned)
{
    if (mState != PlayerStateEnum::STATE_STUNNED && stunned)
    {
        // Check if got stunned
        changeState(PlayerStateEnum::STATE_STUNNED, playerData, collScene, damageTrigger);
        return;
    }

    if (mState == PlayerStateEnum::STATE_IDLE || mState == PlayerStateEnum::STATE_WALKING)
    {
        return;
    }

    mStateTimer -= deltaTime;
    if (mStateTimer <= 0.0f)
    {
        if (mState == PlayerStateEnum::STATE_CASTING)
        {
            // After casting, start fishing
            changeState(PlayerStateEnum::STATE_FISHING, playerData, collScene, damageTrigger);
            return;
        }

        // After other timed states, go back to idle
        changeState(PlayerStateEnum::STATE_IDLE, playerData, collScene, damageTrigger);
    }
}
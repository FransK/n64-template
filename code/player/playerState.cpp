#include "playerState.h"

#include "../animation/animationComponent.h"
#include "../math/vector3.h"
#include "../scene/fish.h"

void PlayerState::changeState(const PlayerStateEnum &newState, PlayerData &playerData, Collision::Scene &collScene, Collision::Collider *damageTrigger)
{
    if (mState == newState)
    {
        return;
    }

    if (mState == STATE_ATTACKING)
    {
        collScene.deactivate(damageTrigger);
    }

    if (mState == STATE_FISHING && mActionSuccess)
    {
        *mFishCaught += 1;
        mActionSuccess = false;
    }

    mState = newState;

    switch (mState)
    {
    case STATE_ATTACKING:
        mStateTimer = SHOVE_TIME;
        playerData.velocity = {0.0f, 0.0f, 0.0f};
        playerData.attackPosition = {playerData.position.x + playerData.rotation.x * ATTACK_OFFSET,
                                     playerData.position.y + PlayerColliderType.data.cylinder.halfHeight,
                                     playerData.position.z + -playerData.rotation.y * ATTACK_OFFSET};
        collScene.activate(damageTrigger);
        mAnimationComponent->play_animation(Anim::SHOVE);
        break;
    case STATE_CASTING:
        mStateTimer = CAST_TIME;
        playerData.velocity = {0.0f, 0.0f, 0.0f};
        mAnimationComponent->play_animation(Anim::CAST);
        break;
    case STATE_FISHING:
        mStateTimer = Fish::get_new_timer();
        playerData.velocity = {0.0f, 0.0f, 0.0f};
        mAnimationComponent->play_animation(Anim::IDLE);
        break;
    case STATE_STUNNED:
        mStateTimer = RECEIVE_SHOVE_TIME;
        playerData.velocity = {0.0f, 0.0f, 0.0f};
        mAnimationComponent->play_animation(Anim::RECEIVE_SHOVE);
        break;
    case STATE_IDLE:
        mStateTimer = 0.0f;
        playerData.velocity = {0.0f, 0.0f, 0.0f};
        mAnimationComponent->play_animation(Anim::IDLE);
        break;
    case STATE_WALKING:
        mStateTimer = 0.0f;
        mAnimationComponent->play_animation(Anim::RUN);
        break;
    }
}

void PlayerState::update(float deltaTime, PlayerData &playerData, Collision::Scene &collScene, Collision::Collider *damageTrigger, bool stunned)
{
    if (mState != STATE_STUNNED && stunned)
    {
        // Check if got stunned
        changeState(STATE_STUNNED, playerData, collScene, damageTrigger);
        return;
    }

    if (mState == STATE_IDLE || mState == STATE_WALKING)
    {
        return;
    }

    mStateTimer -= deltaTime;
    if (mStateTimer <= 0.0f)
    {
        if (mState == STATE_CASTING)
        {
            // After casting, start fishing
            changeState(STATE_FISHING, playerData, collScene, damageTrigger);
            return;
        }

        // After other timed states, go back to idle
        changeState(STATE_IDLE, playerData, collScene, damageTrigger);
    }
}

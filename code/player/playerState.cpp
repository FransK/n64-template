#include "playerState.h"

#include "../animation/animationComponent.h"
#include "../math/vector3.h"
#include "../scene/fish.h"

void PlayerState::changeState(const PlayerStateEnum &newState, Vector3 &velocity)
{
    if (mState == newState)
    {
        return;
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
        velocity = {0.0f, 0.0f, 0.0f};
        mAnimationComponent->play_animation(Anim::SHOVE);
        break;
    case STATE_CASTING:
        mStateTimer = CAST_TIME;
        velocity = {0.0f, 0.0f, 0.0f};
        mAnimationComponent->play_animation(Anim::CAST);
        break;
    case STATE_FISHING:
        mStateTimer = Fish::get_new_timer();
        velocity = {0.0f, 0.0f, 0.0f};
        break;
    case STATE_STUNNED:
        mStateTimer = RECEIVE_SHOVE_TIME;
        velocity = {0.0f, 0.0f, 0.0f};
        mAnimationComponent->play_animation(Anim::RECEIVE_SHOVE);
        break;
    case STATE_IDLE:
        mStateTimer = 0.0f;
        velocity = {0.0f, 0.0f, 0.0f};
        mAnimationComponent->play_animation(Anim::IDLE);
        break;
    case STATE_WALKING:
        mStateTimer = 0.0f;
        mAnimationComponent->play_animation(Anim::RUN);
        break;
    }
}

void PlayerState::update(float deltaTime, Vector3 &velocity)
{
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
            changeState(STATE_FISHING, velocity);
            return;
        }

        // After other timed states, go back to idle
        changeState(STATE_IDLE, velocity);
    }
}

#include "playerAi.h"

using namespace Fishing;

void PlayerAi::change_state(AIState newState)
{
    // Cleanup current state
    switch (mCurrentState)
    {
    case STATE_IDLE:
        break;
    case STATE_MOVE_TO_PLAYER:
        break;
    case STATE_ATTACK:
        break;
    case STATE_MOVE_TO_FISH:
        break;
    case STATE_FISH:
        break;
    case STATE_ANIMATION_LOCKED:
        break;
    }

    mCurrentState = newState;

    switch (mCurrentState)
    {
    case STATE_IDLE:
        mInputState.move = {0.0f, 0.0f};
        mInputState.fish = false;
        mInputState.attack = false;
        break;
    case STATE_MOVE_TO_PLAYER:
        mInputState.move = {0.0f, 0.0f};
        mInputState.fish = false;
        mInputState.attack = false;
        break;
    case STATE_ATTACK:
        mInputState.move = {0.0f, 0.0f};
        mInputState.fish = false;
        mInputState.attack = true;
        break;
    case STATE_MOVE_TO_FISH:
        mInputState.move = {0.0f, 0.0f};
        mInputState.fish = false;
        mInputState.attack = false;
        break;
    case STATE_FISH:
        mInputState.move = {0.0f, 0.0f};
        mInputState.fish = true;
        mInputState.attack = false;
        mDelayCatchTimer = 0.9f;
        break;
    case STATE_ANIMATION_LOCKED:
        mInputState.move = {0.0f, 0.0f};
        mInputState.fish = false;
        mInputState.attack = false;
        break;
    }
}

void PlayerAi::update_idle(float deltaTime)
{
    if (mDelayActionTimer > 0.0f)
    {
        mDelayActionTimer -= deltaTime;
        return;
    }

    switch (mBehavior)
    {
    case BEHAVE_BALANCED:
        break;
    case BEHAVE_BULLY:
        mNextState = STATE_MOVE_TO_PLAYER;
        break;
    case BEHAVE_FISHERMAN:
        mMovementTarget = mPlayer.get_closest_fish();
        mNextState = STATE_MOVE_TO_FISH;
        break;
    }
}

void PlayerAi::update_move_to_player(float deltaTime)
{
    mMovementTarget = mTarget->get_position();

    Vector3 position = mPlayer.get_position();
    Vector3 distance;
    Vector3::sub(&position, &mMovementTarget, &distance);

    if (Vector3::magSqrd(&distance) < powf(ATTACK_RADIUS + HITBOX_RADIUS, 2))
    {
        mNextState = STATE_ATTACK;
    }
    else
    {
        move_to_target();
    }
}

void PlayerAi::update_attack(float deltaTime)
{
    if (mPlayer.can_move())
    {
        Vector3 position = mPlayer.get_position();
        Vector3 distance;
        Vector3::sub(&mMovementTarget, &position, &distance);

        float direction = atan2f(distance.x, distance.z);
        mTarget->receive_shove();

        mNextState = STATE_ANIMATION_LOCKED;
        mAnimationLockedTimer = SHOVE_TIME;
        mDelayActionTimer = 5.0f;
        return;
    }
}

void PlayerAi::update_move_to_fish(float deltaTime)
{
    Vector3 position = mPlayer.get_position();
    Vector3 distance;
    Vector3::sub(&position, &mMovementTarget, &distance);

    if (Vector3::magSqrd(&distance) < 0.01f)
    {
        mNextState = STATE_FISH;
    }
    else
    {
        move_to_target();
    }
}

void PlayerAi::update_fish(float deltaTime)
{
    if (!mPlayer.is_fishing())
    {
        mInputState = {
            .move = {0.0f, 0.0f},
            .fish = true,
            .attack = false,
        };
        return;
    }

    mInputState = {
        .move = {0.0f, 0.0f},
        .fish = false,
        .attack = false,
    };

    if (mPlayer.is_catchable())
    {
        mDelayCatchTimer -= deltaTime;
        if (mDelayCatchTimer <= 0.0f)
        {
            mInputState = {
                .move = {0.0f, 0.0f},
                .fish = true,
                .attack = false,
            };
            mDelayActionTimer = 3.0f;
            mNextState = STATE_IDLE;
        }
    }
}

void PlayerAi::update_animation_locked(float deltaTime)
{
    mAnimationLockedTimer -= deltaTime;
    if (mAnimationLockedTimer <= 0.0f)
    {
        mNextState = STATE_IDLE;
    }
}

void PlayerAi::move_to_target()
{
    Vector3 position = mPlayer.get_position();
    Vector3 direction;
    Vector3::sub(&mMovementTarget, &position, &direction);
    Vector3::normAndScale(&direction, mSpeed, &direction);

    mInputState = {
        .move = {direction.x, -direction.z},
        .fish = false,
        .attack = false,
    };
}

PlayerAi::PlayerAi(Collision::Scene *scene, T3DModel *model, AIBehavior behavior) : mPlayer(Player(scene, model)),
                                                                                    mCurrentState(AIState::STATE_IDLE),
                                                                                    mNextState(AIState::STATE_IDLE),
                                                                                    mBehavior(behavior)
{
}

void PlayerAi::init(int8_t playerNumber, T3DVec3 position, Vector2 rotation, color_t color)
{
    mPlayer.init(playerNumber, position, rotation, color);
}

Player *PlayerAi::get_player()
{
    return &mPlayer;
}

void PlayerAi::receive_shove()
{
    mNextState = STATE_ANIMATION_LOCKED;
    mAnimationLockedTimer = RECEIVE_SHOVE_TIME;
    mPlayer.receive_shove();
}

void PlayerAi::update_fixed(float deltaTime, Player *currentLeader)
{
    mTarget = currentLeader;

    if (mCurrentState != mNextState)
    {
        change_state(mNextState);
    }

    mPlayer.update_fixed(deltaTime, mInputState);
}

void PlayerAi::update(float deltaTime)
{
    switch (mCurrentState)
    {
    case STATE_IDLE:
        update_idle(deltaTime);
        break;
    case STATE_MOVE_TO_PLAYER:
        update_move_to_player(deltaTime);
        break;
    case STATE_ATTACK:
        update_attack(deltaTime);
        break;
    case STATE_MOVE_TO_FISH:
        update_move_to_fish(deltaTime);
        break;
    case STATE_FISH:
        update_fish(deltaTime);
        break;
    case STATE_ANIMATION_LOCKED:
        update_animation_locked(deltaTime);
        break;
    }

    mPlayer.update(deltaTime, mInputState);
}

#include "playerAi.h"

#include "../../core.h"
#include "collider.h"

void PlayerAi::update(float deltaTime, const PlayerState &playerState, int playerNumber, PlayerData *players, uint8_t *winners)
{
    // Reset input state
    mInputState = {
        .move = {0.0f, 0.0f},
        .fish = false,
        .attack = false,
    };

    if (mDelayActionTimer > 0.0f)
    {
        mDelayActionTimer -= deltaTime;
        return;
    }

    PlayerStateEnum state = playerState.getState();

    switch (state)
    {
    case PlayerStateEnum::STATE_IDLE:
        // Find something to do
        update_idle(deltaTime, playerNumber, players, winners);
    case PlayerStateEnum::STATE_WALKING:
        // Move towards target and perform actions
        update_movement_target();
        move_to_target();
        break;
    case PlayerStateEnum::STATE_FISHING:
        // Check fishing status
        if (playerState.getStateTimer() < CATCH_TIMER - mDelayCatchTimer)
        {
            mInputState.fish = true;
            mDelayActionTimer = 2.0f;
            mDelayCatchTimer = 0.6f;
        }
        break;
    case PlayerStateEnum::STATE_ATTACKING:
        mDelayActionTimer = 5.0f;
        break;
    case PlayerStateEnum::STATE_CASTING:
    case PlayerStateEnum::STATE_STUNNED:
    default:
        // Do nothing, wait for action to complete
        break;
    }
}

void PlayerAi::update_idle(float deltaTime, int playerNumber, PlayerData *players, uint8_t *winners)
{
    switch (mBehavior)
    {
    case AIBehavior::BEHAVE_BALANCED:
        // Alternate between fish and players
        if (mTarget)
        {
            mTarget = nullptr;
            mMovementTarget = find_closest_fish();
        }
        else
        {
            mTarget = find_winner_target(playerNumber, players, winners);
            if (!mTarget)
            {
                mMovementTarget = find_closest_fish();
            }
        }
        break;
    case AIBehavior::BEHAVE_BULLY:
        mTarget = find_winner_target(playerNumber, players, winners);
        if (!mTarget)
        {
            mMovementTarget = find_closest_fish();
        }
        break;
    case AIBehavior::BEHAVE_FISHERMAN:
        mMovementTarget = find_closest_fish();
        break;
    }
}

void PlayerAi::update_movement_target()
{
    if (mTarget)
    {
        mMovementTarget = mTarget->getPosition();
    }
}

void PlayerAi::move_to_target()
{
    Vector3 position = mPlayer->getPosition();
    Vector3 distance;
    Vector3::sub(&mMovementTarget, &position, &distance);

    float distanceThreshold = mTarget ? HITBOX_RADIUS * HITBOX_RADIUS : 1.f;
    if (Vector3::magSqrd(&distance) < distanceThreshold)
    {
        mInputState.move = {0.0f, 0.0f};
        mInputState.fish = mTarget == nullptr;
        mInputState.attack = mTarget != nullptr;
        return;
    }

    Vector3 direction;
    Vector3::normAndScale(&distance, BASE_SPEED, &direction);

    mInputState = {
        .move = {direction.x, -direction.z},
        .fish = false,
        .attack = false,
    };
}

PlayerData *PlayerAi::find_winner_target(int playerNumber, PlayerData *players, uint8_t *winners) const
{
    for (int i = 0; i < Core::MAX_PLAYERS; i++)
    {
        if (i == playerNumber)
        {
            continue;
        }

        if (winners[i])
        {
            return &players[i];
        }
    }
    return nullptr;
}

Vector3 PlayerAi::find_closest_fish() const
{
    Vector3 position = mPlayer->getPosition();
    Vector3 closestFish;
    Vector3::normAndScale(&position, Collision::PLAYING_R, &closestFish);
    return closestFish;
}
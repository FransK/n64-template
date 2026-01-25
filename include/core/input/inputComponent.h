#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include <libdragon.h>

#include "collision/CollisionScene.h"

#include "inputState.h"
#include "playerData.h"
#include "playerState.h"

template <typename InputStrategy>
class InputComponent
{
public:
    InputComponent(InputStrategy inputStrategy) : mInputStrategy(inputStrategy) {}

    void update(float deltaTime,
                PlayerState &playerState,
                PlayerData &playerData,
                Collision::CollisionScene &collScene,
                Collision::Collider *damageTrigger,
                bool stunned)
    {
        InputState inputState = mInputStrategy.update();

        playerState.update(deltaTime, playerData, collScene, damageTrigger, stunned);

        switch (playerState.getState())
        {
        case PlayerStateEnum::STATE_STUNNED:
        case PlayerStateEnum::STATE_CASTING:
        case PlayerStateEnum::STATE_ATTACKING:
            // Ignore inputs
            break;
        case PlayerStateEnum::STATE_FISHING:
            // Allow only fishing inputs
            if (inputState.fish)
            {
                // Stop fishing and check for success
                if (playerState.getStateTimer() <= CATCH_TIMER)
                {
                    // Successful catch
                    playerState.setActionSuccess(true);
                }
                playerState.changeState(PlayerStateEnum::STATE_IDLE, playerData, collScene, damageTrigger);
            }
            break;
        case PlayerStateEnum::STATE_WALKING:
        case PlayerStateEnum::STATE_IDLE:
            // Process all inputs
            if (inputState.attack)
            {
                // Start attack
                playerState.changeState(PlayerStateEnum::STATE_ATTACKING, playerData, collScene, damageTrigger);
                break;
            }

            if (inputState.fish)
            {
                // Start fishing
                playerState.changeState(PlayerStateEnum::STATE_CASTING, playerData, collScene, damageTrigger);
                break;
            }

            if (abs(inputState.move.x) > MIN_MOVE_INPUT || abs(inputState.move.y) > MIN_MOVE_INPUT)
            {
                // Normalize rotation vector
                Vector2 normMove{};
                Vector2::norm(&inputState.move, &normMove);
                // Start walking
                playerState.changeState(PlayerStateEnum::STATE_WALKING, playerData, collScene, damageTrigger);
                playerData.setRotation({normMove.x, normMove.y});
                playerData.setVelocity({normMove.x * BASE_SPEED, 0.0f, -normMove.y * BASE_SPEED});
                break;
            }

            playerState.changeState(PlayerStateEnum::STATE_IDLE, playerData, collScene, damageTrigger);
            break;
        default:
            break;
        }
    }

private:
    InputStrategy mInputStrategy{};
};

#endif
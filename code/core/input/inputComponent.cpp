#include "inputComponent.h"

void InputComponent::update(float deltaTime,
                            InputState &inputState,
                            PlayerState &playerState,
                            PlayerData &playerData,
                            Collision::Scene &collScene,
                            Collision::Collider *damageTrigger,
                            bool stunned)
{
    playerState.update(deltaTime, playerData, collScene, damageTrigger, stunned);

    switch (playerState.getState())
    {
    case STATE_STUNNED:
    case STATE_CASTING:
    case STATE_ATTACKING:
        // Ignore inputs
        break;
    case STATE_FISHING:
        // Allow only fishing inputs
        if (inputState.fish)
        {
            // Stop fishing and check for success
            if (playerState.getStateTimer() <= CATCH_TIMER)
            {
                // Successful catch
                playerState.setActionSuccess(true);
            }
            playerState.changeState(STATE_IDLE, playerData, collScene, damageTrigger);
        }
        break;
    case STATE_WALKING:
    case STATE_IDLE:
        // Process all inputs
        if (inputState.attack)
        {
            // Start attack
            playerState.changeState(STATE_ATTACKING, playerData, collScene, damageTrigger);
            break;
        }

        if (inputState.fish)
        {
            // Start fishing
            playerState.changeState(STATE_CASTING, playerData, collScene, damageTrigger);
            break;
        }

        if (abs(inputState.move.x) > MIN_MOVE_INPUT || abs(inputState.move.y) > MIN_MOVE_INPUT)
        {
            // Normalize rotation vector
            Vector2 normMove{};
            Vector2::norm(&inputState.move, &normMove);
            // Start walking
            playerState.changeState(STATE_WALKING, playerData, collScene, damageTrigger);
            playerData.rotation = {normMove.x, normMove.y};
            playerData.velocity = {normMove.x * BASE_SPEED, 0.0f, -normMove.y * BASE_SPEED};
            break;
        }

        playerState.changeState(STATE_IDLE, playerData, collScene, damageTrigger);
        break;
    default:
        break;
    }
}
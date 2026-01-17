#include "inputComponent.h"

void InputComponent::updateInputPlayer(float deltaTime,
                                       joypad_port_t port,
                                       PlayerState &playerState,
                                       Vector3 &playerVelocity,
                                       Vector2 &playerRotation)
{
    auto btn = joypad_get_buttons_pressed(port);
    auto inputs = joypad_get_inputs(port);

    mInputState = {
        .move = {(float)inputs.stick_x, (float)inputs.stick_y},
        .fish = btn.a != 0,
        .attack = btn.b != 0};

    updateCommon(deltaTime, playerState, playerVelocity, playerRotation);
}

void InputComponent::updateInputAI(float deltaTime,
                                   InputState &inputState,
                                   PlayerState &playerState,
                                   Vector3 &playerVelocity,
                                   Vector2 &playerRotation)
{
    mInputState = inputState;

    updateCommon(deltaTime, playerState, playerVelocity, playerRotation);
}

void InputComponent::updateCommon(float deltaTime,
                                  PlayerState &playerState,
                                  Vector3 &playerVelocity,
                                  Vector2 &playerRotation)
{
    playerState.update(deltaTime, playerVelocity);

    switch (playerState.getState())
    {
    case STATE_STUNNED:
    case STATE_CASTING:
    case STATE_ATTACKING:
        // Ignore inputs
        break;
    case STATE_FISHING:
        // Allow only fishing inputs
        if (mInputState.fish)
        {
            // Stop fishing and check for success
            if (playerState.getStateTimer() <= CATCH_TIMER)
            {
                // Successful catch
                playerState.setActionSuccess(true);
            }
            playerState.changeState(STATE_IDLE, playerVelocity);
        }
        break;
    case STATE_WALKING:
    case STATE_IDLE:
        // Process all inputs
        if (mInputState.attack)
        {
            // Start attack
            playerState.changeState(STATE_ATTACKING, playerVelocity);
            break;
        }

        if (mInputState.fish)
        {
            // Start fishing
            playerState.changeState(STATE_CASTING, playerVelocity);
            break;
        }

        if (abs(mInputState.move.x) > MIN_MOVE_INPUT || abs(mInputState.move.y) > MIN_MOVE_INPUT)
        {
            // Normalize rotation vector
            Vector2 normMove{};
            Vector2::norm(&mInputState.move, &normMove);
            // Start walking
            playerState.changeState(STATE_WALKING, playerVelocity);
            playerRotation = {-normMove.y, normMove.x};
            playerVelocity = {normMove.x * BASE_SPEED, 0.0f, -normMove.y * BASE_SPEED};
            break;
        }

        playerState.changeState(STATE_IDLE, playerVelocity);
        break;
    default:
        break;
    }
}
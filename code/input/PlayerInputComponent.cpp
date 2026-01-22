#include "PlayerInputComponent.h"

void PlayerInputComponent::update(float deltaTime,
                                  InputState &inputState,
                                  PlayerState &playerState,
                                  PlayerData &playerData,
                                  Collision::Scene &collScene,
                                  Collision::Collider *damageTrigger,
                                  bool stunned)
{
    auto btn = joypad_get_buttons_pressed(mPort);
    auto inputs = joypad_get_inputs(mPort);

    inputState = {
        .move = {(float)inputs.stick_x, (float)inputs.stick_y},
        .fish = btn.a != 0,
        .attack = btn.b != 0};

    debugf("PlayerInputComponent: move=(%.2f, %.2f), fish=%d, attack=%d\n",
           inputState.move.x,
           inputState.move.y,
           inputState.fish,
           inputState.attack);

    InputComponent::update(deltaTime, inputState, playerState, playerData, collScene, damageTrigger, stunned);
}
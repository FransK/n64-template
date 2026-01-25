#include "PlayerInputStrategy.h"

InputState PlayerInputStrategy::update()
{
    auto btn = joypad_get_buttons_pressed(mPort);
    auto inputs = joypad_get_inputs(mPort);

    InputState inputState{
        .move = {(float)inputs.stick_x, (float)inputs.stick_y},
        .fish = btn.a != 0,
        .attack = btn.b != 0};

    return inputState;
}
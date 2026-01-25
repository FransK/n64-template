#ifndef PLAYER_INPUT_STRATEGY_H
#define PLAYER_INPUT_STRATEGY_H

#include <libdragon.h>
#include "input/inputState.h"
#include <functional>

class PlayerInputStrategy
{
public:
    explicit PlayerInputStrategy(joypad_port_t port) : mPort(port) {}

    InputState update();

private:
    joypad_port_t mPort;
};

#endif
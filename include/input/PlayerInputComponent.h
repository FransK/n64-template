#ifndef PLAYER_INPUT_COMPONENT_H
#define PLAYER_INPUT_COMPONENT_H

#include "input/inputComponent.h"
#include <functional>

class PlayerInputComponent : public InputComponent
{
public:
    PlayerInputComponent(joypad_port_t port) : mPort(port) {}

    void update(float deltaTime,
                InputState &inputState,
                PlayerState &playerState,
                PlayerData &playerData,
                Collision::CollisionScene &collScene,
                Collision::Collider *damageTrigger,
                bool stunned) override;

private:
    joypad_port_t mPort;
};

#endif
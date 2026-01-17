#ifndef INPUT_COMPONENT
#define INPUT_COMPONENT

#include <libdragon.h>

#include "inputState.h"
#include "../player/playerData.h"
#include "../player/playerState.h"

class InputComponent
{
public:
    InputComponent() : mInputState(InputState{{0.0f, 0.0f}, false, false}) {}

    void updateInputPlayer(float deltaTime,
                           joypad_port_t port,
                           PlayerState &playerState,
                           Vector3 &playerVelocity,
                           Vector2 &playerRotation);
    void updateInputAI(float deltaTime,
                       InputState &inputState,
                       PlayerState &playerState,
                       Vector3 &playerVelocity,
                       Vector2 &playerRotation);

private:
    InputState mInputState{};

    void updateCommon(float deltaTime,
                      PlayerState &playerState,
                      Vector3 &playerVelocity,
                      Vector2 &playerRotation);
};

#endif
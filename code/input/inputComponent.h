#ifndef INPUT_COMPONENT
#define INPUT_COMPONENT

#include <libdragon.h>

#include "inputState.h"
#include "../collision/scene.h"
#include "../player/playerData.h"
#include "../player/playerState.h"

class InputComponent
{
public:
    InputComponent() : mInputState(InputState{{0.0f, 0.0f}, false, false}) {}

    void updateInputPlayer(float deltaTime,
                           joypad_port_t port,
                           PlayerState &playerState,
                           PlayerData &playerData,
                           Collision::Scene &collScene,
                           Collision::Collider *damageTrigger);
    void updateInputAI(float deltaTime,
                       InputState &inputState,
                       PlayerState &playerState,
                       PlayerData &playerData,
                       Collision::Scene &collScene,
                       Collision::Collider *damageTrigger);

private:
    InputState mInputState{};

    void updateCommon(float deltaTime,
                      PlayerState &playerState,
                      PlayerData &playerData,
                      Collision::Scene &collScene,
                      Collision::Collider *damageTrigger);
};

#endif
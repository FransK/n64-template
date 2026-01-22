#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include <libdragon.h>

#include "collision/CollisionScene.h"

#include "inputState.h"
#include "playerData.h"
#include "playerState.h"

class InputComponent
{
public:
    virtual void update(float deltaTime,
                        InputState &inputState,
                        PlayerState &playerState,
                        PlayerData &playerData,
                        Collision::CollisionScene &collScene,
                        Collision::Collider *damageTrigger,
                        bool stunned);
};

#endif
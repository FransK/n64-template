#ifndef CORE_INPUT_UPDATE_H
#define CORE_INPUT_UPDATE_H

#include "input/InputComponentVariant.h"

struct InputComponentUpdate
{
    struct args
    {
        float deltaTime;
        PlayerState &playerState;
        PlayerData &playerData;
        Collision::CollisionScene &collScene;
        Collision::Collider *damageTrigger;
        bool stunned;
    } args;

    InputComponentUpdate(float deltaTime,
                         PlayerState &playerState,
                         PlayerData &playerData,
                         Collision::CollisionScene &collScene,
                         Collision::Collider *damageTrigger,
                         bool stunned) : args{deltaTime, playerState, playerData, collScene, damageTrigger, stunned} {}

    void operator()(InputComponent<PlayerInputStrategy> &inputComponent)
    {
        inputComponent.update(args.deltaTime,
                              args.playerState,
                              args.playerData,
                              args.collScene,
                              args.damageTrigger,
                              args.stunned);
    }

    void operator()(InputComponent<AIInputStrategy> &inputComponent)
    {
        inputComponent.update(args.deltaTime,
                              args.playerState,
                              args.playerData,
                              args.collScene,
                              args.damageTrigger,
                              args.stunned);
    }
};

#endif // CORE_INPUT_UPDATE_H
#ifndef AI_INPUT_STRATEGY_H
#define AI_INPUT_STRATEGY_H

#include <functional>
#include <libdragon.h>
#include "input/inputState.h"
#include "scene/playerAi.h"

class AIInputStrategy
{
public:
    explicit AIInputStrategy(PlayerAi *ai) : mAi(ai) {}

    InputState update();

private:
    PlayerAi *mAi;
};

#endif
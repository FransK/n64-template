#include "AIInputStrategy.h"

InputState AIInputStrategy::update()
{
    return mAi->get_input_state();
}
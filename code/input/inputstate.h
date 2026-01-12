#pragma once

#include "../math/vector2.h"

using namespace Math;

namespace Fishing
{
    struct InputState
    {
        Vector2 move{};
        bool fish{};
        bool attack{};
    };
}
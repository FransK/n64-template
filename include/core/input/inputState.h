#pragma once

#include "math/vector2.h"

using namespace Math;

constexpr float MIN_MOVE_INPUT = 8.f;
constexpr float ROTATION_INPUT = 26.f;

struct InputState
{
    Vector2 move{};
    bool fish{};
    bool attack{};
};
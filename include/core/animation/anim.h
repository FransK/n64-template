#ifndef ANIM
#define ANIM

#include <cstdint>

enum struct Anim : uint8_t
{
    IDLE = 0,
    RUN,
    SHOVE,
    RECEIVE_SHOVE,
    CAST
};

constexpr float PLAYER_SCALE = 0.125f;

#endif
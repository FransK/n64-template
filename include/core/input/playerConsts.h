#ifndef PLAYER_CONSTS_H
#define PLAYER_CONSTS_H

#include <cstdint>

constexpr float BILLBOARD_YOFFSET = 200.f;

constexpr float HITBOX_RADIUS = 8.f;
constexpr float ATTACK_OFFSET = 8.f;

constexpr int8_t FIRST_PLAYER_COLLIDER_GROUP = 1;

constexpr float BASE_SPEED = 70.f;

constexpr float CATCH_TIMER = 1.f;

constexpr float SHOVE_TIME_SCALE = 2.0f;
constexpr float SHOVE_TIME = 21.0f / (30.0f * SHOVE_TIME_SCALE);
constexpr float RECEIVE_SHOVE_TIME = SHOVE_TIME * 3.0f;
constexpr float CAST_TIME = 21.0f / 30.0f;

#endif
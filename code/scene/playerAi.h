#pragma once

#include "../math/vector2.h"
#include "../math/vector3.h"
#include "../player/playerData.h"
#include "../player/playerState.h"
#include "player.h"

enum AIBehavior : uint8_t
{
    BEHAVE_FISHERMAN,
    BEHAVE_BULLY,
    BEHAVE_BALANCED,
};

class PlayerAi
{
public:
    PlayerAi() = default;
    void init(PlayerData *player)
    {
        mBehavior = BEHAVE_FISHERMAN;
        mPlayer = player;
    }

    void set_behavior(AIBehavior behavior) { mBehavior = behavior; }
    void update(float deltaTime, const PlayerState &state, int playerNumber, PlayerData *players, uint8_t *winners, InputState &out);

private:
    AIBehavior mBehavior;
    Vector3 mMovementTarget{};
    const PlayerData *mPlayer{};
    const PlayerData *mTarget{};
    float mDelayActionTimer{0.0f};
    float mDelayCatchTimer{0.6f};

    void update_idle(float deltaTime, int playerNumber, PlayerData *players, uint8_t *winners, InputState &out);
    void update_movement_target();
    void move_to_target(InputState &out);
    PlayerData *find_winner_target(int playerNumber, PlayerData *players, uint8_t *winners) const;
    Vector3 find_closest_fish() const;
};
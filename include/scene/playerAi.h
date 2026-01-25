#pragma once

#include "math/vector2.h"
#include "math/vector3.h"
#include "input/inputState.h"
#include "input/playerData.h"
#include "input/playerState.h"
#include "player.h"

enum struct AIBehavior : uint8_t
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
        mBehavior = AIBehavior::BEHAVE_FISHERMAN;
        mPlayer = player;
    }

    void set_behavior(AIBehavior behavior) { mBehavior = behavior; }
    void update(float deltaTime, const PlayerState &state, int playerNumber, PlayerData *players, uint8_t *winners);
    InputState get_input_state() const { return mInputState; }

private:
    AIBehavior mBehavior;
    Vector3 mMovementTarget{};
    const PlayerData *mPlayer{};
    const PlayerData *mTarget{};
    float mDelayActionTimer{0.0f};
    float mDelayCatchTimer{0.8f};
    InputState mInputState{};

    void update_idle(float deltaTime, int playerNumber, PlayerData *players, uint8_t *winners);
    void update_movement_target();
    void move_to_target();
    PlayerData *find_winner_target(int playerNumber, PlayerData *players, uint8_t *winners) const;
    Vector3 find_closest_fish() const;
};
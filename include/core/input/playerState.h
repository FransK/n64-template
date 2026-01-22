#ifndef PLAYER_STATE
#define PLAYER_STATE

#include <set>

#include "playerConsts.h"
#include "playerData.h"

#include "Observer.h"
#include "scene.h"
#include "vector3.h"

enum PlayerStateEnum
{
    STATE_IDLE,
    STATE_WALKING,
    STATE_ATTACKING,
    STATE_STUNNED,
    STATE_CASTING,
    STATE_FISHING,
};

class PlayerState
{
public:
    using PlayerStateObserver = Observer<PlayerState>;

    PlayerState() : mState(STATE_IDLE),
                    mStateTimer(0.0f),
                    mFishCaught(nullptr),
                    mActionSuccess(false) {}

    PlayerState(int *fishCaught) : mState(STATE_IDLE),
                                   mStateTimer(0.0f),
                                   mFishCaught(fishCaught),
                                   mActionSuccess(false) {}

    bool canCatch() const { return mState == STATE_FISHING && mStateTimer < CATCH_TIMER; }
    void changeState(const PlayerStateEnum &newState, PlayerData &playerData, Collision::Scene &collScene, Collision::Collider *damageTrigger);
    PlayerStateEnum getState() const { return mState; }
    float getStateTimer() const { return mStateTimer; }
    void init(int *fishCaught);
    void reset();
    void setActionSuccess(bool success) { mActionSuccess = success; }
    void update(float deltaTime, PlayerData &playerData, Collision::Scene &collScene, Collision::Collider *damageTrigger, bool stunned);

    bool attach(PlayerStateObserver *observer);
    bool detach(PlayerStateObserver *observer);
    void notify();

private:
    PlayerStateEnum mState{};
    float mStateTimer{};
    int *mFishCaught{};
    bool mActionSuccess{};
    std::set<PlayerStateObserver *> mObservers{};
};

#endif
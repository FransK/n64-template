#ifndef PLAYER_STATE
#define PLAYER_STATE

#include "playerConsts.h"
#include "playerData.h"

#include "CollisionScene.h"
#include "Observable.h"
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

class PlayerState : public Observable<PlayerState>
{
public:
    PlayerState() : mState(STATE_IDLE),
                    mStateTimer(0.0f),
                    mFishCaught(nullptr),
                    mActionSuccess(false) {}

    PlayerState(int *fishCaught) : mState(STATE_IDLE),
                                   mStateTimer(0.0f),
                                   mFishCaught(fishCaught),
                                   mActionSuccess(false) {}

    bool canCatch() const { return mState == STATE_FISHING && mStateTimer < CATCH_TIMER; }
    void changeState(const PlayerStateEnum &newState, PlayerData &playerData, Collision::CollisionScene &collScene, Collision::Collider *damageTrigger);
    PlayerStateEnum getState() const { return mState; }
    float getStateTimer() const { return mStateTimer; }
    void init(int *fishCaught);
    void reset();
    void setActionSuccess(bool success) { mActionSuccess = success; }
    void update(float deltaTime, PlayerData &playerData, Collision::CollisionScene &collScene, Collision::Collider *damageTrigger, bool stunned);

private:
    PlayerStateEnum mState{};
    float mStateTimer{};
    int *mFishCaught{};
    bool mActionSuccess{};
};

#endif
#ifndef PLAYER_STATE
#define PLAYER_STATE

#include "playerConsts.h"
#include "playerData.h"

#include "CollisionScene.h"
#include "Observable.h"
#include "vector3.h"

enum struct PlayerStateEnum
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
    bool canCatch() const { return mState == PlayerStateEnum::STATE_FISHING && mStateTimer < CATCH_TIMER; }
    PlayerStateEnum getState() const { return mState; }
    int getFishCaught() const { return mFishCaught; }
    float getStateTimer() const { return mStateTimer; }
    void setActionSuccess(bool success) { mActionSuccess = success; }

    void changeState(const PlayerStateEnum &newState, PlayerData &playerData, Collision::CollisionScene &collScene, Collision::Collider *damageTrigger);
    void reset();
    void update(float deltaTime, PlayerData &playerData, Collision::CollisionScene &collScene, Collision::Collider *damageTrigger, bool stunned);

private:
    PlayerStateEnum mState{PlayerStateEnum::STATE_IDLE};
    float mStateTimer{};
    int mFishCaught{};
    bool mActionSuccess{false};
};

#endif
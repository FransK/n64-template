#ifndef PLAYER_STATE
#define PLAYER_STATE

#include "playerConsts.h"
#include "playerData.h"

#include "animationComponent.h"
#include "collision/scene.h"
#include "math/vector3.h"

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
    PlayerState() : mState(STATE_IDLE),
                    mStateTimer(0.0f),
                    mFishCaught(nullptr),
                    mAnimationComponent(nullptr),
                    mActionSuccess(false) {}

    PlayerState(int *fishCaught, AnimationComponent *animationComponent) : mState(STATE_IDLE),
                                                                           mStateTimer(0.0f),
                                                                           mFishCaught(fishCaught),
                                                                           mAnimationComponent(animationComponent),
                                                                           mActionSuccess(false) {}

    bool canCatch() const { return mState == STATE_FISHING && mStateTimer < CATCH_TIMER; }
    void changeState(const PlayerStateEnum &newState, PlayerData &playerData, Collision::Scene &collScene, Collision::Collider *damageTrigger);
    PlayerStateEnum getState() const { return mState; }
    float getStateTimer() const { return mStateTimer; }
    void init(int *fishCaught, AnimationComponent *animationComponent);
    void reset();
    void setActionSuccess(bool success) { mActionSuccess = success; }
    void update(float deltaTime, PlayerData &playerData, Collision::Scene &collScene, Collision::Collider *damageTrigger, bool stunned);

private:
    PlayerStateEnum mState{};
    float mStateTimer{};
    int *mFishCaught{};
    AnimationComponent *mAnimationComponent{};
    bool mActionSuccess{};
};

#endif
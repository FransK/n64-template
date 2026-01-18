#ifndef PLAYER_STATE
#define PLAYER_STATE

#include "playerConsts.h"
#include "playerData.h"

#include "../animation/animationComponent.h"
#include "../collision/scene.h"
#include "../math/vector3.h"
#include "../scene/playerColliders.h"

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

    void init(int *fishCaught, AnimationComponent *animationComponent)
    {
        mFishCaught = fishCaught;
        mAnimationComponent = animationComponent;
    }

    void reset()
    {
        mState = STATE_IDLE;
        mStateTimer = 0.0f;
        mActionSuccess = false;
    }

    bool canCatch() const { return mState == STATE_FISHING && mStateTimer < CATCH_TIMER; }
    void changeState(const PlayerStateEnum &newState, PlayerData &playerData, Collision::Scene &collScene, Collision::Collider *damageTrigger);
    void setActionSuccess(bool success) { mActionSuccess = success; }
    void update(float deltaTime, PlayerData &playerData, Collision::Scene &collScene, Collision::Collider *damageTrigger, bool stunned);

    PlayerStateEnum getState() const { return mState; }
    float getStateTimer() const { return mStateTimer; }

private:
    PlayerStateEnum mState{};
    float mStateTimer{};
    int *mFishCaught{};
    AnimationComponent *mAnimationComponent{};
    bool mActionSuccess{};
};

#endif
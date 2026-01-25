#ifndef ANIMATION_COMPONENT
#define ANIMATION_COMPONENT

#include <t3d/t3danim.h>

#include "math/vector2.h"
#include "math/vector3.h"
#include "playerState.h"

#include "anim.h"

using namespace Math;

/*
Animation Component
-
Encapsulates both animation and rendering
*/
class AnimationComponent : public Observer<PlayerState>
{
public:
    AnimationComponent(T3DModel *model, PlayerState *playerState, color_t primColor);
    ~AnimationComponent();

    AnimationComponent(const AnimationComponent &) = delete;
    AnimationComponent &operator=(const AnimationComponent &) = delete;
    AnimationComponent(AnimationComponent &&) = default;
    AnimationComponent &operator=(AnimationComponent &&) = default;

    uint32_t get_vert_count() const { return mModel->totalVertCount; }
    void update(float deltaTime);
    void draw(const Vector3 &position, const Vector2 &rotation) const;

private:
    T3DModel *mModel{};
    T3DMat4FP *mModelMatFP{};
    rspq_block_t *mDplPlayer{};

    T3DAnim mAnimIdle{};
    T3DAnim mAnimPunch{};
    T3DAnim mAnimReceiveHit{};
    T3DAnim mAnimRun{};
    T3DAnim mAnimCast{};
    T3DAnim *mActiveAnim{};
    T3DSkeleton mSkeleton{};

    color_t mPrimColor{};

    void on_player_state_change(const PlayerState &state);
    void play_animation(Anim anim);
};

void update(AnimationComponent &comp, float deltaTime);
void draw(const AnimationComponent &comp, const Vector3 &position, const Vector2 &rotation);

#endif
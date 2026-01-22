#ifndef ANIMATION_COMPONENT
#define ANIMATION_COMPONENT

#include <t3d/t3danim.h>

#include "math/vector2.h"
#include "math/vector3.h"

#include "anim.h"

using namespace Math;

/*
Animation Component
-
Encapsulates both animation and rendering
*/
class AnimationComponent
{
public:
    AnimationComponent() = default;
    ~AnimationComponent();

    void init(T3DModel *model, color_t primColor);
    void update(float deltaTime);
    void play_animation(Anim anim);
    void update_animation(float deltaTime);
    uint32_t draw(const Vector3 &position, const Vector2 &rotation) const;

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
};

#endif
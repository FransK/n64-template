#include "animation/animationComponent.h"
#include "math/quaternion.h"

AnimationComponent::AnimationComponent(T3DModel *model, PlayerState *playerState, color_t primColor)
    : Observer<PlayerState>([this](const PlayerState &state)
                            { this->on_player_state_change(state); }),
      mModel(model),
      mPrimColor(primColor)
{
    // Model Credits: Quaternius (CC0) https://quaternius.com/
    mSkeleton = t3d_skeleton_create(mModel);

    mAnimIdle = t3d_anim_create(mModel, "Idle");
    t3d_anim_attach(&mAnimIdle, &mSkeleton);

    mAnimRun = t3d_anim_create(mModel, "Run");
    t3d_anim_attach(&mAnimRun, &mSkeleton);
    t3d_anim_set_playing(&mAnimRun, false);

    mAnimPunch = t3d_anim_create(mModel, "Punch");
    t3d_anim_attach(&mAnimPunch, &mSkeleton);
    t3d_anim_set_looping(&mAnimPunch, false);
    t3d_anim_set_playing(&mAnimPunch, false);
    t3d_anim_set_speed(&mAnimPunch, SHOVE_TIME_SCALE);

    mAnimReceiveHit = t3d_anim_create(mModel, "RecieveHit");
    t3d_anim_attach(&mAnimReceiveHit, &mSkeleton);
    t3d_anim_set_looping(&mAnimReceiveHit, false);
    t3d_anim_set_playing(&mAnimReceiveHit, false);

    mAnimCast = t3d_anim_create(mModel, "SwordSlash");
    t3d_anim_attach(&mAnimCast, &mSkeleton);
    t3d_anim_set_looping(&mAnimCast, false);
    t3d_anim_set_playing(&mAnimCast, false);

    mActiveAnim = &mAnimIdle;
    mModelMatFP = (T3DMat4FP *)malloc_uncached(sizeof(T3DMat4FP));

    rspq_block_begin();
    t3d_matrix_push(mModelMatFP);
    rdpq_set_prim_color(mPrimColor);
    t3d_model_draw_skinned(mModel, &mSkeleton);
    t3d_matrix_pop(1);
    mDplPlayer = rspq_block_end();
}

AnimationComponent::~AnimationComponent()
{
    rspq_block_free(mDplPlayer);

    mActiveAnim = nullptr;

    t3d_anim_destroy(&mAnimIdle);
    t3d_anim_destroy(&mAnimRun);
    t3d_anim_destroy(&mAnimPunch);
    t3d_anim_destroy(&mAnimReceiveHit);
    t3d_anim_destroy(&mAnimCast);
    t3d_skeleton_destroy(&mSkeleton);

    free_uncached(mModelMatFP);
}

void AnimationComponent::update(float deltaTime)
{
    t3d_anim_update(mActiveAnim, deltaTime);
    t3d_skeleton_update(&mSkeleton);
}

void AnimationComponent::draw(const Vector3 &position, const Vector2 &rotation) const
{
    // Rotate the complex by 90 degrees to align with T3D's coordinate system
    Math::Vector2 adjustedRotation = {-rotation.y, rotation.x};

    Math::Quaternion mathQuat;
    Math::axisComplex({0.0f, 1.0f, 0.0f}, &adjustedRotation, &mathQuat);

    T3DVec3 scale = {{PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE}};
    T3DQuat quat = {{mathQuat.x, mathQuat.y, mathQuat.z, mathQuat.w}};
    T3DVec3 pos = {{position.x, position.y, position.z}};
    t3d_mat4fp_from_srt(mModelMatFP,
                        scale,
                        quat,
                        pos);

    rspq_block_run(mDplPlayer);
}

void AnimationComponent::on_player_state_change(const PlayerState &state)
{
    switch (state.getState())
    {
    case PlayerStateEnum::STATE_IDLE:
        play_animation(Anim::IDLE);
        break;
    case PlayerStateEnum::STATE_WALKING:
        play_animation(Anim::RUN);
        break;
    case PlayerStateEnum::STATE_ATTACKING:
        play_animation(Anim::SHOVE);
        break;
    case PlayerStateEnum::STATE_STUNNED:
        play_animation(Anim::RECEIVE_SHOVE);
        break;
    case PlayerStateEnum::STATE_CASTING:
        play_animation(Anim::CAST);
        break;
    case PlayerStateEnum::STATE_FISHING:
        play_animation(Anim::IDLE);
        break;
    }
}

void AnimationComponent::play_animation(Anim anim)
{
    t3d_anim_set_playing(&mAnimIdle, anim == Anim::IDLE);
    t3d_anim_set_playing(&mAnimRun, anim == Anim::RUN);
    t3d_anim_set_playing(&mAnimPunch, anim == Anim::SHOVE);
    t3d_anim_set_playing(&mAnimReceiveHit, anim == Anim::RECEIVE_SHOVE);
    t3d_anim_set_playing(&mAnimCast, anim == Anim::CAST);

    switch (anim)
    {
    case Anim::IDLE:
        mActiveAnim = &mAnimIdle;
        return;
    case Anim::RUN:
        mActiveAnim = &mAnimRun;
        return;
    case Anim::SHOVE:
        t3d_anim_set_time(&mAnimPunch, 0.0f);
        mActiveAnim = &mAnimPunch;
        return;
    case Anim::RECEIVE_SHOVE:
        t3d_anim_set_time(&mAnimReceiveHit, 0.0f);
        mActiveAnim = &mAnimReceiveHit;
        return;
    case Anim::CAST:
        t3d_anim_set_time(&mAnimCast, 0.0f);
        mActiveAnim = &mAnimCast;
        return;
    }
}

void update(AnimationComponent &component, float deltaTime)
{
    component.update(deltaTime);
}

void draw(const AnimationComponent &component, const Vector3 &position, const Vector2 &rotation)
{
    component.draw(position, rotation);
}
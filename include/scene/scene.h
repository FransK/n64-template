#pragma once

#include <memory>
#include <t3d/t3d.h>
#include <vector>

#include "../../core.h"

#include "adapters/t3d.h"
#include "animation/animationComponent.h"
#include "collision/CollisionScene.h"
#include "debug/overlay.h"
#include "input/inputComponent.h"
#include "input/InputComponentVariant.h"
#include "input/playerData.h"
#include "input/playerState.h"

// #include "ComponentVector.h"

#include "camera.h"
#include "player.h"
#include "playerAi.h"

using CollisionScene = ::Collision::CollisionScene;
using namespace Core;

constexpr float INTRO_TIME = 3.f;
constexpr float GAME_TIME = 60.f;
constexpr float GAME_OVER_TIME = 3.f;

constexpr uint8_t COLOR_AMBIENT[4] = {0xAA, 0xAA, 0xAA, 0xFF};
constexpr uint8_t COLOR_DIR[4] = {0xFF, 0xAA, 0xAA, 0xFF};

constexpr color_t COLORS[] = {
    PLAYER_COLOR_1,
    PLAYER_COLOR_2,
    PLAYER_COLOR_3,
    PLAYER_COLOR_4,
};

class Scene
{
public:
    Scene();
    ~Scene();

    long ticksActorUpdate{0};
    long ticksCollisionUpdate{0};
    long ticksAnimationUpdate{0};

    const CollisionScene &getCollScene();
    void update_fixed(float deltaTime);
    void update(float deltaTime);
    void reset();

private:
    enum struct State : uint8_t
    {
        INTRO = 0,
        GAME,
        GAME_OVER
    } mState{};

    float mStateTime{};

    /* Player Data Block - Positions, Velocities, etc*/
    std::array<PlayerData, MAX_PLAYERS> mPlayerData{};
    std::array<PlayerState, MAX_PLAYERS> mPlayerStates{};
    std::array<PlayerAi, MAX_PLAYERS> mAIPlayers{};
    std::vector<InputComponentVariant> mInputComponents;
    CollisionScene mCollisionScene;
    std::vector<AnimationComponent> mAnimationComponents;
    std::array<int, MAX_PLAYERS> mStunnedIds{-1};

    /* Container class? */
    std::array<Player, MAX_PLAYERS> mPlayers{};
    std::array<uint8_t, MAX_PLAYERS> mWinners{0};
    int mCurrTopScore{0};

    Adapters::ModelAdapter mPlayerModel;
    Adapters::ModelAdapter mMapModel;

    surface_t *mCurrentFB{};
    surface_t *mLastFB{};
    T3DMat4FP *mMapMatFP{};
    rspq_block_t *mDplMap{};

    rdpq_font_t *mFontBillboard{};
    rdpq_font_t *mFontText{};

    T3DViewport mViewport{};
    Camera mCamera{};
    T3DVec3 mLightDirVec{};

    Debug::Overlay debugOvl{};
};
#pragma once

#include <t3d/t3d.h>
#include <vector>

#include "../../core.h"

#include "../animation/animationComponent.h"
#include "../collision/scene.h"
#include "../debug/overlay.h"
#include "../input/inputComponent.h"
#include "../player/playerData.h"
#include "../player/playerState.h"

#include "camera.h"
#include "player.h"
#include "playerAi.h"

using CollisionScene = ::Collision::Scene;
using std::vector;
using namespace Core;

namespace Fishing
{
    constexpr float INTRO_TIME = 1.f;
    constexpr float GAME_TIME = 30.f;
    constexpr float GAME_OVER_TIME = 5.f;

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
    private:
        enum class State : uint8_t
        {
            INTRO = 0,
            GAME,
            GAME_OVER
        } mState{};

        float mStateTime{};

        /* Player Data Block - Positions, Velocities, etc*/
        PlayerData mPlayerData[MAX_PLAYERS];
        PlayerState mPlayerStates[MAX_PLAYERS];
        PlayerAi mAIPlayers[MAX_PLAYERS];
        InputComponent mInputComponents[MAX_PLAYERS];
        CollisionScene mCollisionScene;
        AnimationComponent mAnimationComponents[MAX_PLAYERS];
        int mFishCaught[MAX_PLAYERS]{0};
        int mStunnedIds[MAX_PLAYERS]{-1};

        /* Container class? */
        Player mPlayers[MAX_PLAYERS];

        uint8_t mWinners[MAX_PLAYERS]{0};
        int mCurrTopScore{0};

        T3DModel *mPlayerModel{};

        surface_t *mCurrentFB{};
        surface_t *mLastFB{};
        T3DModel *mMapModel{};
        T3DMat4FP *mMapMatFP{};
        rspq_block_t *mDplMap{};

        rdpq_font_t *mFontBillboard{};
        rdpq_font_t *mFontText{};

        T3DViewport mViewport{};
        Camera mCamera{};
        T3DVec3 mLightDirVec{};

        Debug::Overlay debugOvl{};

    public:
        long ticksActorUpdate{0};
        long ticksCollisionUpdate{0};
        long ticksAnimationUpdate{0};

        const CollisionScene &getCollScene();
        void update_fixed(float deltaTime);
        void update(float deltaTime);

        Scene();
        ~Scene();
    };
}
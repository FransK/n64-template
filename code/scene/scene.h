#pragma once

#include <t3d/t3d.h>

#include "../../core.h"

#include "../collision/scene.h"
#include "../debug/overlay.h"
#include "../input/inputstate.h"

#include "camera.h"
#include "player.h"
#include "playerAi.h"

using CollisionScene = ::Collision::Scene;
using namespace Core;

namespace Fishing
{
    constexpr float INTRO_TIME = 5.f;
    constexpr float GAME_TIME = 30.f;
    constexpr float GAME_OVER_TIME = 5.f;

    constexpr uint8_t COLOR_AMBIENT[4] = {0xAA, 0xAA, 0xAA, 0xFF};
    constexpr uint8_t COLOR_DIR[4] = {0xFF, 0xAA, 0xAA, 0xFF};

    constexpr color_t COLORS[] = {
        PlayerColor1,
        PlayerColor2,
        PlayerColor3,
        PlayerColor4,
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

        CollisionScene mCollisionScene{};

        T3DModel *mPlayerModel{};

        surface_t *mCurrentFB{};
        surface_t *mLastFB{};
        T3DModel *mMapModel{};
        T3DMat4FP *mMapMatFP{};
        rspq_block_t *mDplMap{};

        Player *mPlayers[MaxPlayers]{};
        PlayerAi *mAIPlayers[MaxPlayers]{};
        InputState mInputState[MaxPlayers]{};
        uint8_t mWinners[MaxPlayers]{0};
        uint8_t mCurrTopScore{0};

        rdpq_font_t *mFontBillboard{};
        rdpq_font_t *mFontText{};

        T3DViewport mViewport{};
        Camera mCamera{};
        T3DVec3 mLightDirVec{};

        Debug::Overlay debugOvl{};

        void read_inputs(PlyNum plyNum);
        void process_attacks(PlyNum attacker);

    public:
        long ticksActorUpdate{0};

        const CollisionScene &getCollScene();
        void update_fixed(float deltaTime);
        void update(float deltaTime);

        Scene();
        ~Scene();
    };
}
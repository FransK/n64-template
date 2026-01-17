#include <algorithm>
#include <string>
#include "scene.h"

#include "../config.h"
#include "../debug/debugDraw.h"
#include "../debug/overlay.h"
#include "../math/vector2.h"
#include "timer.h"

namespace Fishing
{
    bool showFPS = false;
    bool debugOverlay = false;

    Scene::Scene()
    {
        mPlayerModel = t3d_model_load(FS_BASE_PATH "player3.t3dm");
        mMapModel = t3d_model_load(FS_BASE_PATH "map.t3dm");

        Debug::init();

        mMapMatFP = (T3DMat4FP *)malloc_uncached(sizeof(T3DMat4FP));
        t3d_mat4fp_from_srt_euler(mMapMatFP, (float[3]){PLAYER_SCALE, 1.0f, PLAYER_SCALE}, (float[3]){0, 0, 0}, (float[3]){0, 0, 0});

        rspq_block_begin();
        t3d_matrix_push(mMapMatFP);
        t3d_model_draw(mMapModel);
        t3d_matrix_pop(1);
        mDplMap = rspq_block_end();

        mFontBillboard = rdpq_font_load(FS_BASE_PATH "squarewave.font64");
        rdpq_text_register_font(FONT_BILLBOARD, mFontBillboard);
        mFontText = rdpq_font_load(FS_BASE_PATH "squarewave.font64");
        rdpq_text_register_font(FONT_TEXT, mFontText);
        for (size_t i = 0; i < MAX_PLAYERS; i++)
        {
            const rdpq_fontstyle_t style{.color = COLORS[i]};
            rdpq_font_style(mFontText, i, &style);
        }
        const rdpq_fontstyle_t countdown_style{.color = RGBA32(255, 255, 255, 255)};
        rdpq_font_style(mFontText, 4, &countdown_style);

        // === Setup viewport and lighting ==== //
        mViewport = t3d_viewport_create();
        mCamera.position = (T3DVec3){{0, 100.0f, 75.0f}};
        mCamera.target = (T3DVec3){{0, 0, 30}};

        mLightDirVec = (T3DVec3){{1.0f, 1.0f, 1.0f}};
        t3d_vec3_norm(&mLightDirVec);

        // === Initialize the players and components === //
        PlayerData initialPositions[MAX_PLAYERS] = {
            {{-25, 0.0f, 0}, {1, 0}},
            {{0, 0.0f, -25}, {0, 1}},
            {{25, 0.0f, 0}, {-1, 0}},
            {{0, 0.0f, 25}, {0, -1}}};

        for (size_t i = 0; i < MAX_PLAYERS; i++)
        {
            mPlayerData[i] = initialPositions[i];
            mPlayerStates[i].init(&mFishCaught[i], &mAnimationComponents[i]);
            mInputComponents[i] = InputComponent();
            mAnimationComponents[i].init(mPlayerModel, COLORS[i]);
            mPlayers[i].init(&mCollisionScene, &mPlayerData[i], &mPlayerStates[i], i);
            mAIPlayers[i].init(&mPlayerData[i]);

            AIBehavior behavior = (i == MAX_PLAYERS - 1) ? AIBehavior::BEHAVE_BULLY : AIBehavior::BEHAVE_FISHERMAN;
            mAIPlayers[i].set_behavior(behavior);
        }

        // === Initialize Game State === //
        mState = State::INTRO;
        mStateTime = INTRO_TIME;

        timer_init();
    }

    Scene::~Scene()
    {
        rspq_block_free(mDplMap);

        t3d_model_free(mPlayerModel);
        t3d_model_free(mMapModel);

        free_uncached(mMapMatFP);

        rdpq_text_unregister_font(FONT_TEXT);
        rdpq_font_free(mFontText);
        rdpq_text_unregister_font(FONT_BILLBOARD);
        rdpq_font_free(mFontBillboard);

        timer_close();
    }

    void Scene::update_fixed(float deltaTime)
    {
        // === Update Game State === //
        mStateTime -= deltaTime;
        switch (mState)
        {
        case State::INTRO:
            if (mStateTime <= 0)
            {
                mState = State::GAME;
                mStateTime = GAME_TIME;
            }
            return;
        case State::GAME:
            if (mStateTime <= 0)
            {
                mState = State::GAME_OVER;
                mStateTime = GAME_OVER_TIME;
            }
            break;
        case State::GAME_OVER:
            if (mStateTime <= 0)
            {
                core_game_end();
            }
            return;
        default:
            return;
        }

        // === Update Inputs and AI === //
        ticksActorUpdate = get_ticks();
        for (size_t i = 0; i < core_get_playercount(); i++)
        {
            mInputComponents[i].updateInputPlayer(deltaTime,
                                                  core_get_playercontroller((PlyNum)i),
                                                  mPlayerStates[i],
                                                  mPlayerData[i],
                                                  mCollisionScene,
                                                  mPlayers[i].get_damage_trigger());
        }
        for (size_t i = core_get_playercount(); i < MAX_PLAYERS; i++)
        {
            // Update AI to get inputs
            InputState aiInput{};
            mAIPlayers[i].update(deltaTime,
                                 mPlayerStates[i],
                                 i,
                                 &mPlayerData[0],
                                 &mWinners[0],
                                 aiInput);

            // Feed AI inputs to input component
            mInputComponents[i].updateInputAI(deltaTime,
                                              aiInput,
                                              mPlayerStates[i],
                                              mPlayerData[i],
                                              mCollisionScene,
                                              mPlayers[i].get_damage_trigger());
        }
        ticksCollisionUpdate = get_ticks();
        ticksActorUpdate = ticksCollisionUpdate - ticksActorUpdate;

        // === Update Collision Scene === //
        mCollisionScene.update(deltaTime);
        ticksAnimationUpdate = get_ticks();
        ticksCollisionUpdate = ticksAnimationUpdate - ticksCollisionUpdate;

        // === Update Animations === //
        for (auto &animComp : mAnimationComponents)
        {
            animComp.update(deltaTime);
        }
        ticksAnimationUpdate = get_ticks() - ticksAnimationUpdate;

        // === Keep Track of Leaders === //
        mCurrTopScore = 0;
        for (auto &p : mFishCaught)
        {
            mCurrTopScore = std::max(mCurrTopScore, p);
        }

        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            mWinners[i] = mFishCaught[i] >= mCurrTopScore;
        }
    }

    void Scene::update(float deltaTime)
    {
        // === Debug Controls === //
        {
            auto ctrl = core_get_playercontroller(PLAYER_1);
            auto btn = joypad_get_buttons_pressed(ctrl);
            auto held = joypad_get_buttons_held(ctrl);

            if (held.z)
            {
                if (btn.d_up)
                    debugOverlay = !debugOverlay;
                if (btn.d_down)
                    showFPS = !showFPS;
            }
        }

        // === Attach RDP === //
        mLastFB = mCurrentFB;
        mCurrentFB = display_get();
        rdpq_attach(mCurrentFB, display_get_zbuf());

        // === Set Camera === //
        mCamera.update(mViewport);

        // === Draw Viewport === //
        t3d_viewport_attach(&mViewport);

        // === Draw Background === //
        rdpq_set_mode_fill({(uint8_t)(0x80),
                            (uint8_t)(0xb8),
                            (uint8_t)(0xd8),
                            0xFF});
        rdpq_fill_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        // === Draw 3D === //
        t3d_frame_start();

        t3d_screen_clear_depth();

        t3d_light_set_ambient(COLOR_AMBIENT);
        t3d_light_set_directional(0, COLOR_DIR, &mLightDirVec);
        t3d_light_set_count(1);

        uint32_t vertices = mMapModel->totalVertCount;
        rspq_block_run(mDplMap);

        // === Draw players (3D Pass) === //
        for (size_t i = 0; i < MAX_PLAYERS; i++)
        {
            vertices += mAnimationComponents[i].draw(mPlayerData[i].position, mPlayerData[i].rotation);
        }

        // === Draw billboards (2D Pass) === //
        for (size_t i = 0; i < MAX_PLAYERS; i++)
        {
            mPlayers[i].draw_billboard(mViewport);
        }

        // === Draw UI === //
        const rdpq_textparms_t center_text_h{
            .style_id = 4,
            .width = (int16_t)display_get_width(),
            .align = ALIGN_CENTER,
        };
        rdpq_text_printf(&center_text_h, FONT_TEXT, 0, TIMER_Y, "%d", (int)ceilf(mStateTime));

        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            const rdpq_textparms_t score_params{
                .style_id = (int16_t)i};
            rdpq_text_printf(&score_params, FONT_TEXT, SCORE_X + i * SCORE_X_SPACING, SCORE_Y, "%d", mFishCaught[i]);
        }

        if (mState == State::GAME_OVER)
        {
            const rdpq_textparms_t center_text_hv{
                .style_id = 4,
                .width = (int16_t)display_get_width(),
                .height = (int16_t)display_get_height(),
                .align = ALIGN_CENTER,
                .valign = VALIGN_CENTER,
            };
            std::string message{};
            for (int i = 0; i < MAX_PLAYERS; i++)
            {
                mWinners[i] = mFishCaught[i] >= mCurrTopScore;
                if (mWinners[i])
                {
                    core_set_winner((PlyNum)i);
                    message += ("Player " + std::to_string(i + 1) + " wins!\n");
                }
            }
            rdpq_text_printf(&center_text_hv, FONT_TEXT, 0, 0, message.c_str());
        }

        // Debug UI
        if (debugOverlay)
        {
            debugOvl.draw(*this, vertices, deltaTime);
            Debug::draw((uint16_t *)mCurrentFB->buffer);

            // Debug::printStart();
            // for (auto &ai : playerAI)
            // {
            //     ai.debugDraw();
            // }
        }
        else if (showFPS)
        {
            Debug::printStart();
            Debug::printf(24, 220, "FPS %.2f", display_get_fps());
        }

        // === Detach and show === //
        rdpq_detach_show();
    }

    const CollisionScene &Scene::getCollScene()
    {
        return mCollisionScene;
    }
}
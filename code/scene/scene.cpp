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
        t3d_mat4fp_from_srt_euler(mMapMatFP, (float[3]){0.13f, 1.0f, 0.13f}, (float[3]){0, 0, 0}, (float[3]){0, 0, 0});

        rspq_block_begin();
        t3d_matrix_push(mMapMatFP);
        t3d_model_draw(mMapModel);
        t3d_matrix_pop(1);
        mDplMap = rspq_block_end();

        mFontBillboard = rdpq_font_load(FS_BASE_PATH "squarewave.font64");
        rdpq_text_register_font(FONT_BILLBOARD, mFontBillboard);
        mFontText = rdpq_font_load(FS_BASE_PATH "squarewave.font64");
        rdpq_text_register_font(FONT_TEXT, mFontText);
        for (size_t i = 0; i < MAXPLAYERS; i++)
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

        // === Initialize the players === //
        T3DVec3 startPositions[] = {
            (T3DVec3){{-25, 0.0f, 0}},
            (T3DVec3){{0, 0.0f, -25}},
            (T3DVec3){{25, 0.0f, 0}},
            (T3DVec3){{0, 0.0f, 25}},
        };

        Vector2 startRotations[] = {
            {0, 1},
            {1, 0},
            {0, -1},
            {-1, 0}};

        for (uint16_t i = 0; i < core_get_playercount(); i++)
        {
            mPlayers[i] = new Player(&mCollisionScene, mPlayerModel);
            mPlayers[i]->init(i, startPositions[i], startRotations[i], COLORS[i]);
        }
        for (uint16_t i = core_get_playercount(); i < MAXPLAYERS; i++)
        {
            AIBehavior behavior = (i == MAXPLAYERS - 1) ? AIBehavior::BEHAVE_BULLY : AIBehavior::BEHAVE_FISHERMAN;
            mAIPlayers[i] = new PlayerAi(&mCollisionScene, mPlayerModel, behavior);
            mAIPlayers[i]->init(i, startPositions[i], startRotations[i], COLORS[i]);
            mPlayers[i] = mAIPlayers[i]->get_player();
        }

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

        for (size_t i = 0; i < core_get_playercount(); i++)
        {
            delete mPlayers[i];
            mPlayers[i] = nullptr;
        }
        for (size_t i = core_get_playercount(); i < MAXPLAYERS; i++)
        {
            delete mAIPlayers[i];
            mAIPlayers[i] = nullptr;
        }

        timer_close();
    }

    void Scene::read_inputs(PlyNum plyNum)
    {
        joypad_port_t port = core_get_playercontroller(plyNum);
        auto btn = joypad_get_buttons_pressed(port);
        auto inputs = joypad_get_inputs(port);

        mInputState[plyNum] = {
            .move = {(float)inputs.stick_x, (float)inputs.stick_y},
            .fish = btn.a != 0,
            .attack = btn.b != 0};
    }

    void Scene::process_attacks(PlyNum attacker)
    {
        if (!mPlayers[attacker]->can_move())
        {
            return;
        }

        for (size_t i = 0; i < MAXPLAYERS; i++)
        {
            if (attacker == i)
            {
                mPlayers[i]->shove();
                continue;
            }

            Vector2 attack_pos{};
            mPlayers[attacker]->get_attack_position(attack_pos);

            float pos_diff[] = {
                mPlayers[i]->get_position().x - attack_pos.x,
                mPlayers[i]->get_position().z - attack_pos.y,
            };

            float square_distance = pos_diff[0] * pos_diff[0] + pos_diff[1] * pos_diff[1];

            if (square_distance < powf((ATTACK_RADIUS + HITBOX_RADIUS), 2))
            {
                if (i < core_get_playercount())
                {
                    mPlayers[i]->receive_shove();
                }
                else
                {
                    mAIPlayers[i]->receive_shove();
                }
            }
        }
    }

    void Scene::update_fixed(float deltaTime)
    {
        // === Update State === //
        mStateTime -= deltaTime;
        switch (mState)
        {
        case State::INTRO:
            if (mStateTime <= 0)
            {
                mState = State::GAME;
                mStateTime = GAME_TIME;
            }
            break;
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

        // === Update Fixed Players === //
        for (size_t i = 0; i < core_get_playercount(); i++)
        {
            mPlayers[i]->update_fixed(deltaTime, mInputState[i]);
        }
        for (size_t i = core_get_playercount(); i < MAXPLAYERS; i++)
        {
            mAIPlayers[i]->update_fixed(deltaTime, mPlayers[0]); // TODO
        }

        // === Update Collision Scene === //
        mCollisionScene.update(deltaTime);

        // === Keep Track of Leaders === //
        mCurrTopScore = 0;
        for (auto &p : mPlayers)
        {
            mCurrTopScore = std::max(mCurrTopScore, p->get_fish_caught());
        }

        for (int i = 0; i < MAXPLAYERS; i++)
        {
            mWinners[i] = mPlayers[i]->get_fish_caught() >= mCurrTopScore;
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

        // === Process Inputs === //
        ticksActorUpdate = get_ticks();
        if (mState == State::GAME)
        {
            for (size_t i = 0; i < core_get_playercount(); i++)
            {
                read_inputs((PlyNum)i);
                if (mInputState[i].attack)
                {
                    process_attacks((PlyNum)i);
                }
            }
        }

        // === Update Players === //
        if (mState == State::GAME)
        {
            for (size_t i = 0; i < core_get_playercount(); i++)
            {
                mPlayers[i]->update(deltaTime, mInputState[i]);
            }
            for (size_t i = core_get_playercount(); i < MAXPLAYERS; i++)
            {
                mAIPlayers[i]->update(deltaTime);
            }
        }
        ticksActorUpdate = get_ticks() - ticksActorUpdate;

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
        for (size_t i = 0; i < MAXPLAYERS; i++)
        {
            vertices += mPlayers[i]->draw(mViewport, mCamera.position);
        }

        // === Draw billboards (2D Pass) === //
        for (size_t i = 0; i < MAXPLAYERS; i++)
        {
            mPlayers[i]->draw_billboard(mViewport, mCamera.position);
        }

        // === Draw UI === //
        const rdpq_textparms_t center_text_h{
            .style_id = 4,
            .width = (int16_t)display_get_width(),
            .align = ALIGN_CENTER,
        };
        rdpq_text_printf(&center_text_h, FONT_TEXT, 0, TIMER_Y, "%d", (int)ceilf(mStateTime));

        for (int i = 0; i < MAXPLAYERS; i++)
        {
            const rdpq_textparms_t score_params{
                .style_id = (int16_t)i};
            rdpq_text_printf(&score_params, FONT_TEXT, SCORE_X + i * SCORE_X_SPACING, SCORE_Y, "%d", mPlayers[i]->get_fish_caught());
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
            for (int i = 0; i < MAXPLAYERS; i++)
            {
                mWinners[i] = mPlayers[i]->get_fish_caught() >= mCurrTopScore;
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
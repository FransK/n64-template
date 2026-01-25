#include <algorithm>
#include <string>
#include <variant>
#include "timer.h"

#include "scene.h"
#include "config.h"
#include "debug/debugDraw.h"
#include "debug/overlay.h"
#include "input/AIInputStrategy.h"
#include "input/PlayerInputStrategy.h"
#include "input/InputComponentUpdate.h"
#include "math/vector2.h"

bool showFPS = false;
bool debugOverlay = false;

constexpr std::string_view FS_BASE = "rom:/";

const std::string playerPath = std::string(FS_BASE) + "player3.t3dm";
const std::string mapPath = std::string(FS_BASE) + "map.t3dm";

Scene::Scene()
    : mPlayerModel(playerPath), mMapModel(mapPath)
{
    Debug::init();

    mMapMatFP = (T3DMat4FP *)malloc_uncached(sizeof(T3DMat4FP));
    t3d_mat4fp_from_srt_euler(mMapMatFP, (float[3]){PLAYER_SCALE, 1.0f, PLAYER_SCALE}, (float[3]){0, 0, 0}, (float[3]){0, 0, 0});

    rspq_block_begin();
    t3d_matrix_push(mMapMatFP);
    t3d_model_draw(mMapModel.getModel());
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
    Vector3 initialPositions[MAX_PLAYERS] = {
        {-25, 0.0f, 0},
        {0, 0.0f, -25},
        {25, 0.0f, 0},
        {0, 0.0f, 25}};
    Vector2 initialRotations[MAX_PLAYERS] = {
        {1, 0},
        {0, -1},
        {-1, 0},
        {0, 1}};

    mInputComponents.reserve(MAX_PLAYERS);
    mAnimationComponents.reserve(MAX_PLAYERS);

    for (size_t i = 0; i < MAX_PLAYERS; i++)
    {
        mPlayerData[i].setPosition(initialPositions[i]);
        mPlayerData[i].setRotation(initialRotations[i]);
        mPlayers[i].init(&mCollisionScene, &mPlayerData[i], &mPlayerStates[i], i);
        mAIPlayers[i].init(&mPlayerData[i]);

        if (i < core_get_playercount())
        {
            mInputComponents.emplace_back(PlayerInputStrategy((joypad_port_t)i));
        }
        else
        {
            mInputComponents.emplace_back(AIInputStrategy(&mAIPlayers[i]));
        }

        AIBehavior behavior = (i == MAX_PLAYERS - 1) ? AIBehavior::BEHAVE_BULLY : AIBehavior::BEHAVE_FISHERMAN;
        mAIPlayers[i].set_behavior(behavior);

        mAnimationComponents.emplace_back(mPlayerModel.getModel(), &mPlayerStates[i], COLORS[i]);
        mPlayerStates[i].attach(&mAnimationComponents.back());
    }

    // === Initialize Game State === //
    mState = State::INTRO;
    mStateTime = INTRO_TIME;

    timer_init();
}

Scene::~Scene()
{
    rspq_block_free(mDplMap);

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
            Core::core_game_end();
        }
        return;
    default:
        return;
    }

    // === Update Inputs and AI === //
    ticksActorUpdate = get_ticks();
    bool stunnedThisFrame[MAX_PLAYERS] = {false, false, false, false};
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        for (int j = 0; j < MAX_PLAYERS; j++)
        {
            if (mStunnedIds[i] == mPlayers[j].get_collider()->entityId)
            {
                stunnedThisFrame[j] = true;
                break;
            }
        }
        mStunnedIds[i] = -1;
    }

    for (size_t i = core_get_playercount(); i < MAX_PLAYERS; i++)
    {
        mAIPlayers[i].update(deltaTime, mPlayerStates[i], i, mPlayerData.data(), mWinners.data());
    }

    for (size_t i = 0; i < MAX_PLAYERS; i++)
    {
        std::visit(InputComponentUpdate{deltaTime,
                                        mPlayerStates[i],
                                        mPlayerData[i],
                                        mCollisionScene,
                                        mPlayers[i].get_damage_trigger(),
                                        stunnedThisFrame[i]},
                   mInputComponents[i]);
    }
    ticksCollisionUpdate = get_ticks();
    ticksActorUpdate = ticksCollisionUpdate - ticksActorUpdate;

    // === Update Collision Scene === //
    mCollisionScene.update(deltaTime, mStunnedIds.data());
    ticksCollisionUpdate = get_ticks() - ticksCollisionUpdate;

    // === Keep Track of Leaders === //
    mCurrTopScore = 0;
    for (auto &state : mPlayerStates)
    {
        mCurrTopScore = std::max(mCurrTopScore, state.getFishCaught());
    }

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        mWinners[i] = mPlayerStates[i].getFishCaught() >= mCurrTopScore;
    }
}

void Scene::update(float deltaTime)
{
    // === Debug Controls === //
    {
        auto ctrl = core_get_playercontroller(PlyNum::PLAYER_1);
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

    // === Update Animations === //
    ticksAnimationUpdate = get_ticks();
    for (auto &animComp : mAnimationComponents)
    {
        animComp.update(deltaTime);
    }
    ticksAnimationUpdate = get_ticks() - ticksAnimationUpdate;

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

    uint32_t vertices = mMapModel.getModel()->totalVertCount;
    rspq_block_run(mDplMap);

    // === Draw players (3D Pass) === //
    for (size_t i = 0; i < MAX_PLAYERS; i++)
    {
        mAnimationComponents[i].draw(mPlayerData[i].getPosition(), mPlayerData[i].getRotation());
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
        const rdpq_textparms_t score_params{.style_id = (int16_t)i};
        rdpq_text_printf(&score_params,
                         FONT_TEXT,
                         SCORE_X + i * SCORE_X_SPACING,
                         SCORE_Y,
                         "%d",
                         mPlayerStates[i].getFishCaught());
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
            mWinners[i] = mPlayerStates[i].getFishCaught() >= mCurrTopScore;
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

void Scene::reset()
{
    // Reset player positions and rotations
    Vector3 initialPositions[MAX_PLAYERS] = {
        {-25, 0.0f, 0},
        {0, 0.0f, -25},
        {25, 0.0f, 0},
        {0, 0.0f, 25}};
    Vector2 initialRotations[MAX_PLAYERS] = {
        {1, 0},
        {0, -1},
        {-1, 0},
        {0, 1}};

    for (size_t i = 0; i < MAX_PLAYERS; i++)
    {
        mPlayerData[i].setPosition(initialPositions[i]);
        mPlayerData[i].setRotation(initialRotations[i]);
        mPlayerStates[i].reset();
        mWinners[i] = 0;
        mStunnedIds[i] = -1;
    }

    // Reset game state
    mState = State::INTRO;
    mStateTime = INTRO_TIME;
    mCurrTopScore = 0;
}
#ifndef N64BASE_CONFIG_H
#define N64BASE_CONFIG_H

#include <libdragon.h>

namespace Core
{
    // Filesystem base path
#define FS_BASE_PATH "rom:/"

    // Player number definition
    enum struct PlyNum
    {
        PLAYER_1 = 0,
        PLAYER_2 = 1,
        PLAYER_3 = 2,
        PLAYER_4 = 3,
    };

    // AI difficulty definition
    enum struct AiDiff
    {
        DIFF_EASY = 0,
        DIFF_MEDIUM = 1,
        DIFF_HARD = 2,
    };

    // Player ports
    typedef struct
    {
        joypad_port_t port;
    } PlayerJoypad;

    /***************************************************************
                         Debug Core Constants
    ***************************************************************/

    // Enable Debug features (driven by -DDEBUG=1 from the Makefile)
#if defined(DEBUG) && (DEBUG + 0)
    constexpr bool DEBUG_ENABLED = true;
#else
    constexpr bool DEBUG_ENABLED = false;
#endif

    // Initialize USB and isViewer logging
    constexpr bool DEBUG_LOG = DEBUG_ENABLED;

    // Initialize RDP debugger. Also requires DEBUG_LOG.
    constexpr bool DEBUG_RDP = false && DEBUG_LOG;

    // Skip asking the number of players and assume PLAYER_COUNT
    constexpr bool SKIP_PLAYER_SELECTION = true;

    // The number of human players
    constexpr int PLAYER_COUNT = 1;

    // Skip asking the AI difficulty and assume AI_DIFFICULTY
    constexpr bool SKIP_DIFFICULTY_SELECTION = true;

    // The difficulty of the AI.
    constexpr AiDiff AI_DIFFICULTY = AiDiff::DIFF_EASY;

    /***************************************************************
                         Public Core Constants
    ***************************************************************/

    constexpr color_t PLAYER_COLOR_1 = RGBA32(255, 0, 0, 255);
    constexpr color_t PLAYER_COLOR_2 = RGBA32(0, 255, 0, 255);
    constexpr color_t PLAYER_COLOR_3 = RGBA32(0, 0, 255, 255);
    constexpr color_t PLAYER_COLOR_4 = RGBA32(255, 255, 0, 255);

    constexpr float TICK_RATE = 60;
    constexpr float DELTA_TIME = (1.0f / (double)TICK_RATE);
    constexpr uint8_t MAX_PLAYERS = 4;

    constexpr uint32_t SCREEN_WIDTH = 320;
    constexpr uint32_t SCREEN_HEIGHT = 240;
    constexpr uint32_t TIMER_Y = 210;
    constexpr uint32_t SCORE_X = 50;
    constexpr uint32_t SCORE_Y = 220;
    constexpr uint32_t SCORE_X_SPACING = (SCREEN_WIDTH - 2 * SCORE_X) / (Core::MAX_PLAYERS - 1);

    constexpr int FONT_BILLBOARD = 1;
    constexpr int FONT_TEXT = 2;
}

#endif // N64BASE_CONFIG_H
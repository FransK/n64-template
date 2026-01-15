
#ifndef N64BASE_CONFIG_H
#define N64BASE_CONFIG_H

#include "defs.h"

namespace Core
{
    // Enable Debug features
    constexpr bool DEBUG = true;

    // Initialize USB and isViewer logging
    constexpr bool DEBUG_LOG = DEBUG;

    // Initialize RDP debugger. Also requires DEBUG_LOG.
    constexpr bool DEBUG_RDP = false && DEBUG_LOG;

    // Skip asking the number of players and assume PLAYER_COUNT
    constexpr bool SKIP_PLAYER_SELECTION = true;

    // The number of human players
    constexpr int PLAYER_COUNT = 1;

    // Skip asking the AI difficulty and assume AI_DIFFICULTY
    constexpr bool SKIP_DIFFICULTY_SELECTION = true;

    // The difficulty of the AI.
    constexpr AiDiff AI_DIFFICULTY = DIFF_EASY;
}

#endif
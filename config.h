
#ifndef N64BASE_CONFIG_H
#define N64BASE_CONFIG_H

#include "defs.h"

namespace Core
{
    // Enable Debug features
    constexpr bool Debug = true;

    // Initialize USB and isViewer logging
    constexpr bool DebugLog = Debug;

    // Initialize RDP debugger. Also requires DebugLog.
    constexpr bool DebugRDP = false && DebugLog;

    // Skip asking the number of players and assume PlayerCount
    constexpr bool SkipPlayerSelection = true;

    // The number of human players
    constexpr int PlayerCount = 1;

    // Skip asking the AI difficulty and assume AiDifficulty
    constexpr bool SkipDifficultySelection = true;

    // The difficulty of the AI.
    constexpr AiDiff AiDifficulty = DIFF_EASY;
}

#endif
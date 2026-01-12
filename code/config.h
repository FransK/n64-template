#pragma once

#include <libdragon.h>

#include "../core.h"

namespace Fishing
{
    constexpr uint32_t SCREEN_WIDTH = 320;
    constexpr uint32_t SCREEN_HEIGHT = 240;
    constexpr uint32_t TIMER_Y = 210;
    constexpr uint32_t SCORE_X = 50;
    constexpr uint32_t SCORE_Y = 220;
    constexpr uint32_t SCORE_X_SPACING = (SCREEN_WIDTH - 2 * SCORE_X) / (Core::MAXPLAYERS - 1);

    constexpr int FONT_BILLBOARD = 1;
    constexpr int FONT_TEXT = 2;
}
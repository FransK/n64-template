#ifndef DEFS_H
#define DEFS_H

#include <libdragon.h>

namespace Core
{
    // Player number definition
    typedef enum
    {
        PLAYER_1 = 0,
        PLAYER_2 = 1,
        PLAYER_3 = 2,
        PLAYER_4 = 3,
    } PlyNum;

    // AI difficulty definition
    typedef enum
    {
        DIFF_EASY = 0,
        DIFF_MEDIUM = 1,
        DIFF_HARD = 2,
    } AiDiff;

    // Player ports
    typedef struct
    {
        joypad_port_t port;
    } Player;
}

#endif
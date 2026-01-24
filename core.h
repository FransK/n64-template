#ifndef N64BASE_CORE_H
#define N64BASE_CORE_H

#include <libdragon.h>

#include "config.h"

namespace Core
{
    /***************************************************************
                          Public Core Functions
     ***************************************************************/

    /*==============================
        core_get_playercount
        Get the number of human players
        @return The number of players
    ==============================*/
    uint32_t core_get_playercount();

    /*==============================
        core_get_playercontroller
        Get the controller port of this player.
        Because player 1's controller might not be plugged
        into port number 1.
        @param  The player we want
        @return The controller port
    ==============================*/
    joypad_port_t core_get_playercontroller(PlyNum ply);

    /*==============================
        core_get_aidifficulty
        Gets the current AI difficulty
        @return The AI difficulty
    ==============================*/
    AiDiff core_get_aidifficulty();

    /*==============================
        core_get_subtick
        Gets the current subtick. Use this to help smooth
        movements in your draw loop.
        @return The current subtick, as a
                percentage (0.0f to 1.0f)
    ==============================*/
    double core_get_subtick();

    /*==============================
        core_set_winner
        Set the winner of the minigame. You can call this
        multiple times to set multiple winners.
        @param  The winning player
    ==============================*/
    void core_set_winner(PlyNum ply);

    /*==============================
        game_end
        Use this to mark your minigame as finished
    ==============================*/
    void core_game_end();

    void core_set_playercount(bool *enabledconts);
    void core_set_aidifficulty(AiDiff difficulty);
    void core_set_subtick(double subtick);
    void core_reset_winners();

    // Player info
    extern PlayerJoypad global_core_players[JOYPAD_PORT_COUNT];
    extern uint32_t global_core_playercount;
    extern AiDiff global_core_aidifficulty;

    // Game info
    extern bool global_core_playeriswinner[MAX_PLAYERS];
    extern bool global_game_ending;

    // Core info
    extern double global_core_subtick;
};

#endif
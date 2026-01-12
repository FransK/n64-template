#include "core.h"

uint32_t Core::core_get_playercount()
{
    return global_core_playercount;
}

joypad_port_t Core::core_get_playercontroller(PlyNum ply)
{
    return global_core_players[ply].port;
}

Core::AiDiff Core::core_get_aidifficulty()
{
    return global_core_aidifficulty;
}

double Core::core_get_subtick()
{
    return global_core_subtick;
}

void Core::core_set_winner(PlyNum ply)
{
    global_core_playeriswinner[ply] = true;
}

void Core::core_game_end()
{
    global_game_ending = true;
}

void Core::core_set_playercount(bool *enabledconts)
{
    int plynum = 0;

    // Attempt to find the first N left-most available controllers
    for (int i = 0; i < MAXPLAYERS; i++)
    {
        if (enabledconts[i])
        {
            global_core_players[plynum].port = static_cast<joypad_port_t>(i);
            plynum++;
        }
    }
    global_core_playercount = plynum;
}

void Core::core_set_aidifficulty(AiDiff difficulty)
{
    global_core_aidifficulty = difficulty;
}

void Core::core_set_subtick(double subtick)
{
    global_core_subtick = subtick;
}

void Core::core_reset_winners()
{
    for (int i = 0; i < MAXPLAYERS; i++)
        global_core_playeriswinner[i] = false;
}

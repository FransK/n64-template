#include "core.h"

namespace Core
{
    double global_core_subtick = 0.0;
    bool global_core_playeriswinner[MAX_PLAYERS] = {false, false, false, false};
    AiDiff global_core_aidifficulty = AI_DIFFICULTY;
    uint32_t global_core_playercount = 1;
    PlayerJoypad global_core_players[JOYPAD_PORT_COUNT] = {};
    bool global_game_ending = false;
} // namespace Core

uint32_t Core::core_get_playercount()
{
    return global_core_playercount;
}

joypad_port_t Core::core_get_playercontroller(PlyNum ply)
{
    return global_core_players[static_cast<int>(ply)].port;
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
    global_core_playeriswinner[static_cast<int>(ply)] = true;
}

void Core::core_game_end()
{
    global_game_ending = true;
}

void Core::core_set_playercount(bool *enabledconts)
{
    int plynum = 0;

    // Attempt to find the first N left-most available controllers
    for (int i = 0; i < MAX_PLAYERS; i++)
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
    for (int i = 0; i < MAX_PLAYERS; i++)
        global_core_playeriswinner[i] = false;
}

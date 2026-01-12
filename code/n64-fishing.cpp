#include "n64-fishing.h"

namespace Fishing
{
    Scene *scene = nullptr;
}

/*==============================
    init
    The game initialization function
  ==============================*/
void Fishing::init()
{
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    t3d_init((T3DInitParams){});

    scene = new Fishing::Scene();
}

/*==============================
    fixedloop
    Code that is called every loop, at a fixed delta time.
    Use this function for stuff where a fixed delta time is
    important, like physics.
    @param  The fixed delta time for this tick
==============================*/
void Fishing::fixedloop(float deltatime)
{
    scene->update_fixed(deltatime);
}

/*==============================
    loop
    Code that is called every loop.
    @param  The delta time for this tick
==============================*/
void Fishing::loop(float deltatime)
{
    scene->update(deltatime);
}

/*==============================
    minigame_cleanup
    Clean up any memory used by your game just before it ends.
==============================*/
void Fishing::cleanup()
{
    delete scene;

    // Display cleanup
    t3d_destroy();
    display_close();
}
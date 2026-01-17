#include "n64-fishing.h"

using namespace Fishing;

World::World()
{
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    t3d_init((T3DInitParams){});

    scene = new Scene();
}

World::~World()
{
    delete scene;

    t3d_destroy();
    display_close();
}

/*==============================
    fixedloop
    Code that is called every loop, at a fixed delta time.
    Use this function for stuff where a fixed delta time is
    important, like physics.
    @param  The fixed delta time for this tick
==============================*/
void World::fixedloop(float deltatime)
{
    scene->update_fixed(deltatime);
}

/*==============================
    loop
    Code that is called every loop.
    @param  The delta time for this tick
==============================*/
void World::loop(float deltatime)
{
    scene->update(deltatime);
}
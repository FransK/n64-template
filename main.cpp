#include <libdragon.h>
#include <rspq_profile.h>
#include <t3d/t3d.h>
#include <memory>
#include "World.h"

static void advance_rng()
{
    (void)rand(); // Explicitly discard return value
}

constexpr float TickRate = 60;
constexpr float DeltaTime = (1.0f / TickRate);

int main()
{
#if !defined(NDEBUG)
    debug_init_isviewer();
    debug_init_usblog();
#endif

    // Initialize most subsystems
    asset_init_compression(2);
    asset_init_compression(3);
    wav64_init_compression(3);
    dfs_init(DFS_DEFAULT_LOCATION);

    joypad_init();
    timer_init();
    rdpq_init();
    audio_init(32000, 3);
    mixer_init(32);

    // Enable RDP debugging
#if !defined(NDEBUG)
    rdpq_debug_start();
    rdpq_debug_log(true);
    rspq_profile_start();
#endif

    // Initialize the random number generator, then call rand() every
    // frame so to get random behavior also in emulators.
    uint32_t seed;
    getentropy(&seed, sizeof(seed));
    srand(seed);
    register_VI_handler(advance_rng);

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    t3d_init((T3DInitParams){});

    // Load game
    std::unique_ptr<n64::World> world = std::make_unique<n64::World>();

    while (1)
    {
        world->loadNextScene();

        float accumulator = 0;

        // Program loop
        while (world->nextSceneId() == n64::INVALID_SCENE_ID)
        {
            float frametime = display_get_delta_time();

            // In order to prevent problems if the game slows down significantly,
            // clamp the maximum timestep the simulation can take
            if (frametime > 0.25f)
                frametime = 0.25f;

            // Perform the update in discrete steps (ticks)
            accumulator += frametime;
            while (accumulator >= DeltaTime)
            {
                world->fixedloop(DeltaTime);
                accumulator -= DeltaTime;
            }

            // Read controller data
            joypad_poll();
            mixer_try_play();

            // Unfixed loop
            world->loop(frametime);
        }

        world->unloadCurrentScene();
    }

    display_close();
    t3d_destroy();
}
/**
 * @copyright 2024 - Max Bebök
 * @license MIT
 */
#pragma once
#include <t3d/t3dmath.h>
#include <vector>
#include <functional>

namespace Fishing
{
    class Scene;
}

namespace Debug
{
    using FishingScene = ::Fishing::Scene;

    class Overlay
    {
    private:
        struct MenuItem
        {
            const char *text{};
            int value{};
            bool isBool{};
            std::function<void(MenuItem &)> onChange{};
        };

        struct Menu
        {
            std::vector<MenuItem> items{};
            int currIndex;
        };

        Menu menu{};

    public:
        void draw(FishingScene &scene, uint32_t vertCount, float deltaTime);
    };
}
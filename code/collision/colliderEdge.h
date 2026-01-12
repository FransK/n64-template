#pragma once

#include <cstdint>

namespace Collision
{
    struct ColliderEdge
    {
        uint16_t isStartEdge{};
        uint16_t objectIndex{};
        short x{};

        bool operator<(const ColliderEdge &other)
        {
            if (x == other.x)
            {
                // If other is a start edge and this is an end
                return isStartEdge < other.isStartEdge;
            }

            return x < other.x;
        }
    };
}
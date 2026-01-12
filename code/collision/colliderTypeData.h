#pragma once

namespace Collision
{
    union ColliderTypeData
    {
        struct
        {
            float radius{};
        } sphere;
        struct
        {
            float radius{};
            float halfHeight{};
        } cylinder;
    };
}
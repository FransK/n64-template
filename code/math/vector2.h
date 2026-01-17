#pragma once

#include <cmath>

namespace Math
{
    struct Vector2
    {
        float x, y;

        static void norm(Vector2 *in, Vector2 *out)
        {
            float len2 = in->x * in->x + in->y * in->y;
            if (len2 < 1e-20)
            {
                out->x = 0.0f;
                out->y = 0.0f;
                return;
            }

            float invLen = 1.0f / sqrtf(len2);

            out->x = in->x * invLen;
            out->y = in->y * invLen;
        }
    };

    constexpr Vector2 Vec2Right = {1.0f, 0.0f};
    constexpr Vector2 Vec2Up = {0.0f, 1.0f};
    constexpr Vector2 Vec2Zero = {0.0f, 0.0f};
    constexpr Vector2 Vec2One = {1.0f, 1.0f};
}
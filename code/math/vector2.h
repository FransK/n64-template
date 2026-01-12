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

        static void normAndScale(Vector2 *in, Vector2 *out, float scale)
        {
        }
    };
}
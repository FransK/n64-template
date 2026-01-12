#pragma once

#include <cmath>
#include "vector2.h"
#include "vector3.h"

namespace Math
{
    struct Quaternion
    {
        float x, y, z, w;

        static void axisComplex(const Vector3 &axis, Vector2 *complex, Quaternion *out)
        {
            float sinTheta = 0.5f - complex->x * 0.5f;

            if (sinTheta < 0.0f)
            {
                sinTheta = 0.0f;
            }
            else
            {
                sinTheta = sqrtf(sinTheta);

                if (complex->y < 0.0f)
                {
                    sinTheta = -sinTheta;
                }
            }

            float cosTheta = 0.5f + complex->x * 0.5f;

            if (cosTheta < 0.0f)
            {
                cosTheta = 0.0f;
            }
            else
            {
                cosTheta = sqrtf(cosTheta);
            }

            out->x = axis.x * sinTheta;
            out->y = axis.y * sinTheta;
            out->z = axis.z * sinTheta;
            out->w = cosTheta;
        }
    };
}
#pragma once

#include <cmath>
#include "vector3.h"

namespace Math
{
    struct Plane
    {
        Vector3 normal;
        float d;

        static void calculateBarycentricCoords(Vector3 *a, Vector3 *b, Vector3 *c, Vector3 *point, Vector3 *output);
        static void evaluateBarycentricCoords(Vector3 *a, Vector3 *b, Vector3 *c, Vector3 *bary, Vector3 *output);
        static float calculateLerp(Vector3 *a, Vector3 *b, Vector3 *point);
    };
}
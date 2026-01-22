#pragma once

#include <cstdint>

#include "math/vector3.h"

#include "collider.h"
#include "colliderTypeData.h"

using namespace Math;

namespace Collision
{
    constexpr uint16_t MaxSimplexSize = 4;
    constexpr uint16_t MaxGJKIterations = 16;

    struct Simplex
    {
        Vector3 points[MaxSimplexSize];
        Vector3 objectAPoint[MaxSimplexSize];
        int16_t nPoints;

        Simplex() : nPoints(0) {}

        /* Add the point in the Minkowski difference space (A - B) */
        Vector3 *addPoint(Vector3 *aPoint, Vector3 *bPoint);

        void movePoint(int to, int from);
        int check(Vector3 *nextDirection);
    };

    class GJK
    {
    public:
        static int checkForOverlap(Simplex *simplex, Collider *a, Collider *b, const Vector3 *firstDirection);
    };
}
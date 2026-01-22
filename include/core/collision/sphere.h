#pragma once

#include "collider.h"

namespace Collision
{
    class Sphere
    {
    public:
        static void MinkowskiSum(const ColliderTypeData *data, const Vector3 *direction, Vector3 *output);

        static void BoundingBox(ColliderTypeData *data, Vector2 *rotation, Box3D *box);
    };
}
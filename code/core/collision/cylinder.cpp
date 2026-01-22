#include <cmath>

#include "cylinder.h"

using namespace Collision;

constexpr float SQRT_1_2 = 0.707106781f;

void Cylinder::MinkowskiSum(const ColliderTypeData *data, const Vector3 *direction, Vector3 *output)
{
    float absX = fabsf(direction->x);
    float absY = fabsf(direction->y);
    float angleDot = (absX + absY) * SQRT_1_2;

    if (absX > angleDot && absY > angleDot)
    {
        output->x = direction->x > 0.0f ? SQRT_1_2 * data->cylinder.radius : -SQRT_1_2 * data->cylinder.radius;
        output->z = direction->z > 0.0f ? SQRT_1_2 * data->cylinder.radius : -SQRT_1_2 * data->cylinder.radius;
    }
    else if (absX > absY)
    {
        output->x = direction->x > 0.0f ? data->cylinder.radius : -data->cylinder.radius;
        output->z = 0.0f;
    }
    else
    {
        output->x = 0.0f;
        output->z = direction->z > 0.0f ? data->cylinder.radius : -data->cylinder.radius;
    }

    output->y = direction->y > 0.0f ? data->cylinder.halfHeight : -data->cylinder.halfHeight;
}

void Cylinder::BoundingBox(ColliderTypeData *data, Vector2 *rotation, Box3D *box)
{
    box->min.x = -data->cylinder.radius;
    box->min.y = -data->cylinder.halfHeight;
    box->min.z = -data->cylinder.radius;

    box->max.x = data->cylinder.radius;
    box->max.y = data->cylinder.halfHeight;
    box->max.z = data->cylinder.radius;
}
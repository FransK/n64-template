#include "plane.h"

using namespace Math;

void Plane::calculateBarycentricCoords(Vector3 *a, Vector3 *b, Vector3 *c, Vector3 *point, Vector3 *output)
{
    Vector3 v0;
    Vector3 v1;
    Vector3 v2;

    Vector3::sub(b, a, &v0);
    Vector3::sub(c, a, &v1);
    Vector3::sub(point, a, &v2);

    float d00 = Vector3::dot(&v0, &v0);
    float d01 = Vector3::dot(&v0, &v1);
    float d11 = Vector3::dot(&v1, &v1);
    float d20 = Vector3::dot(&v2, &v0);
    float d21 = Vector3::dot(&v2, &v1);

    float denom = d00 * d11 - d01 * d01;

    if (fabsf(denom) < 0.000001f)
    {
        if (d00 > d11)
        {
            // b is other point
            output->y = calculateLerp(a, b, point);
            output->x = 1.0 - output->y;
            output->z = 0.0f;
        }
        else
        {
            // c is other point
            output->z = calculateLerp(a, c, point);
            output->x = 1.0f - output->z;
            output->z = 0.0f;
        }
        return;
    }

    float denomInv = 1.0f / (d00 * d11 - d01 * d01);
    output->y = (d11 * d20 - d01 * d21) * denomInv;
    output->z = (d00 * d21 - d01 * d20) * denomInv;
    output->x = 1.0f - output->y - output->z;
}

void Plane::evaluateBarycentricCoords(Vector3 *a, Vector3 *b, Vector3 *c, Vector3 *bary, Vector3 *output)
{
    Vector3::scale(a, bary->x, output);
    Vector3::addScaled(output, b, bary->y, output);
    Vector3::addScaled(output, c, bary->z, output);
}

float Plane::calculateLerp(struct Vector3 *a, struct Vector3 *b, struct Vector3 *point)
{
    struct Vector3 v0;
    Vector3::sub(b, a, &v0);

    float denom = Vector3::magSqrd(&v0);

    if (denom < 0.00000001f)
    {
        return 0.5f;
    }

    struct Vector3 pointOffset;
    Vector3::sub(point, a, &pointOffset);

    return Vector3::dot(&pointOffset, &v0) / denom;
}
#include "vector3.h"

#include <cmath>

using namespace Math;

void Vector3::add(const Vector3 *a, const Vector3 *b, Vector3 *out)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

float Vector3::dot(const Vector3 *a, const Vector3 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void Vector3::addScaled(const Vector3 *a, const Vector3 *normal, float scale, Vector3 *out)
{
    out->x = a->x + normal->x * scale;
    out->y = a->y + normal->y * scale;
    out->z = a->z + normal->z * scale;
}

void Vector3::cross(const Vector3 *a, const Vector3 *b, Vector3 *out)
{
    out->x = a->y * b->z - a->z * b->y;
    out->y = a->z * b->x - a->x * b->z;
    out->z = a->x * b->y - a->y * b->x;
}

bool Vector3::isZero(const Vector3 *vector)
{
    return vector->x == 0.0f && vector->y == 0.0f && vector->z == 0.0f;
}

float Vector3::magSqrd(const Vector3 *a)
{
    return a->x * a->x + a->y * a->y + a->z * a->z;
}

void Vector3::negate(const Vector3 *in, Vector3 *out)
{
    out->x = -in->x;
    out->y = -in->y;
    out->z = -in->z;
}

void Vector3::normalize(const Vector3 *in, Vector3 *out)
{
    float denom = in->x * in->x + in->y * in->y + in->z * in->z;

    if (denom == 0.0f)
    {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
    }
    else
    {
        float invSqrt = 1.0f / sqrtf(denom);
        scale(in, invSqrt, out);
    }
}

void Vector3::normAndScale(const Vector3 *in, float scale, Vector3 *out)
{
    const float x = in->x;
    const float y = in->y;
    const float z = in->z;

    const float len2 = x * x + y * y + z * z;

    if (len2 <= 1e-20f)
    {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
        return;
    }

    const float invLen = 1.0f / sqrtf(len2);
    const float s = scale * invLen;

    out->x = x * s;
    out->y = y * s;
    out->z = z * s;
}

void Vector3::perp(const Vector3 *a, Vector3 *out)
{
    if (fabsf(a->x) > fabsf(a->z))
    {
        Vector3::cross(a, &Vec3Forward, out);
    }
    else
    {
        Vector3::cross(a, &Vec3Right, out);
    }
}

void Vector3::scale(const Vector3 *in, float scale, Vector3 *out)
{
    out->x = in->x * scale;
    out->y = in->y * scale;
    out->z = in->z * scale;
}

void Vector3::sub(const Vector3 *a, const Vector3 *b, Vector3 *out)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

void Vector3::tripleProduct(const Vector3 *a, const Vector3 *b, const Vector3 *c, Vector3 *out)
{
    Vector3::scale(b, Vector3::dot(a, c), out);
    Vector3::addScaled(out, a, -Vector3::dot(b, c), out);
}

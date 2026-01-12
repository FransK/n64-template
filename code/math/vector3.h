#pragma once

namespace Math
{
    struct Vector3
    {
        float x, y, z;

        static void add(const Vector3 *a, const Vector3 *b, Vector3 *out);
        static void addScaled(const Vector3 *a, const Vector3 *normal, float scale, Vector3 *out);
        static void cross(const Vector3 *a, const Vector3 *b, Vector3 *out);
        static float dot(const Vector3 *a, const Vector3 *b);
        static bool isZero(const Vector3 *a);
        static float magSqrd(const Vector3 *a);
        static void negate(const Vector3 *in, Vector3 *out);
        static void normalize(const Vector3 *in, Vector3 *out);
        static void normAndScale(const Vector3 *in, float scale, Vector3 *out);
        static void perp(const Vector3 *a, Vector3 *out);
        static void scale(const Vector3 *in, float scale, Vector3 *out);
        static void sub(const Vector3 *a, const Vector3 *b, Vector3 *out);
        static void tripleProduct(const Vector3 *a, const Vector3 *b, const Vector3 *c, Vector3 *out);
    };

    constexpr Vector3 Vec3Right = {1.0f, 0.0f, 0.0f};
    constexpr Vector3 Vec3Up = {0.0f, 1.0f, 0.0f};
    constexpr Vector3 Vec3Forward = {0.0f, 0.0f, 1.0f};
    constexpr Vector3 Vec3Zero = {0.0f, 0.0f, 0.0f};
    constexpr Vector3 Vec3One = {1.0f, 1.0f, 1.0f};
}
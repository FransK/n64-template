/**
 * @copyright 2024 - Max Bebök
 * @license MIT
 */
#pragma once
#include <t3d/t3dmath.h>
#include "../math/box3d.h"

using namespace Math;

namespace Debug
{
    void init();

    void drawLine(const T3DVec3 &a, const T3DVec3 &b, color_t color = {0xFF, 0xFF, 0xFF, 0xFF});
    void drawBox(const Box3D &box);
    void drawSphere(const T3DVec3 &center, float radius, color_t color = {0xFF, 0xFF, 0xFF, 0xFF});
    void draw(uint16_t *fb);

    void printStart();
    float print(float x, float y, const char *str);
    float printf(float x, float y, const char *fmt, ...);

    void destroy();
}
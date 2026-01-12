#include "camera.h"

void Camera::update(T3DViewport &viewport)
{
    t3d_viewport_set_projection(viewport, T3D_DEG_TO_RAD(90.0f), 20.0f, 160.0f);
    t3d_viewport_look_at(viewport, position, target, T3DVec3{{0, 1, 0}});
}
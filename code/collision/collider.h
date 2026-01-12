#pragma once

#include <cstdint>

#include "../math/vector2.h"
#include "../math/vector3.h"
#include "../math/box3d.h"

#include "colliderType.h"

using namespace Math;

namespace Collision
{
    constexpr float GRAVITY_CONSTANT = -10.f * 64.f;
    constexpr float PLAYING_R = 60.f;
    constexpr float PLAYING_R2 = PLAYING_R * PLAYING_R;

    enum CollisionLayers
    {
        CollisionLayerTangible = (1 << 0),
    };

    struct Collider
    {
        int entityId{};
        Vector3 position{};
        Vector2 rotation{};
        Vector3 velocity{};
        Vector3 center{};
        ColliderType type{};
        Box3D boundingBox{};
        float scale{};
        bool hasGravity{true};
        bool isFixed{false};
        uint16_t collisionLayers{};
        uint16_t collisionGroup{};

        void update(float timeStep);
        void recalcBB();
        void constrainPosition();
        void minkowskiSumWorld(const Vector3 *direction, Vector3 *output);
    };
}
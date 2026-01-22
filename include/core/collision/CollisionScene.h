#pragma once

#include <vector>

#include "collider.h"
#include "epa.h"

namespace Collision
{
    class CollisionScene
    {
    public:
        CollisionScene() = default;

        void add(Collider *object, bool isActive = true);
        void remove(Collider *object);
        void activate(Collider *object);
        void deactivate(Collider *object);

        void update(float fixedTimeStep, int (&stunnedIds)[4]);
        void debugDraw();

    private:
        std::vector<Collider *> colliders{};
        std::vector<Collider *> activeColliders{};

        void runCollision(int (&stunnedIds)[4]);
        void constrainToWorld(Collider *object);
        void collide(Collider *a, Collider *b, int (&stunnedIds)[4]);
        void correctOverlap(Collider *object, EpaResult *result, float ratio, float friction, float bounce);
        void correctVelocity(Collider *object, EpaResult *result, float ratio, float friction, float bounce);
    };
}
#pragma once

#include <vector>

#include "collider.h"
#include "epa.h"

using namespace Collision;

namespace Collision
{
    class Scene
    {
    private:
        std::vector<Collider *> colliders;

        void runCollision();
        void constrainToWorld(Collider *object);
        void collide(Collider *a, Collider *b);
        void correctOverlap(Collider *object, EpaResult *result, float ratio, float friction, float bounce);
        void correctVelocity(Collider *object, EpaResult *result, float ratio, float friction, float bounce);

    public:
        Scene();
        ~Scene();

        void add(Collider *object);
        void remove(Collider *object);

        void update(float fixedTimeStep);
        void debugDraw();
    };
}
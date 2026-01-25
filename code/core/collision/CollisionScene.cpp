#include "CollisionScene.h"

#include <algorithm>
#include <vector>

#include "debug/debugDraw.h"
#include "math/vector3.h"

#include "colliderEdge.h"
#include "epa.h"
#include "gjk.h"

using namespace Collision;
using namespace Math;

void CollisionScene::add(Collider *object, bool isActive)
{
    colliders.push_back(object);
    if (isActive)
    {
        activeColliders.push_back(object);
    }
}

void CollisionScene::remove(Collider *object)
{
    for (auto iter = activeColliders.begin(); iter != activeColliders.end(); ++iter)
    {
        if (*iter == object)
        {
            activeColliders.erase(iter);
            break;
        }
    }
    for (auto iter = colliders.begin(); iter != colliders.end(); ++iter)
    {
        if (*iter == object)
        {
            return (void)colliders.erase(iter);
        }
    }
}

void CollisionScene::activate(Collider *object)
{
    for (auto *c : activeColliders)
    {
        if (c == object)
        {
            return;
        }
    }

    activeColliders.push_back(object);
}

void CollisionScene::deactivate(Collider *object)
{
    for (auto iter = activeColliders.begin(); iter != activeColliders.end(); ++iter)
    {
        if (*iter == object)
        {
            return (void)activeColliders.erase(iter);
        }
    }
}

void CollisionScene::update(float fixedTimeStep, int *stunnedIds)
{
    /* Integrate objects */
    for (auto *c : activeColliders)
    {
        c->update(fixedTimeStep);
        c->recalcBB();
    }

    /* Solve collisions */
    runCollision(stunnedIds);

    /* Clamp to world */
    for (auto *c : activeColliders)
    {
        constrainToWorld(c);
        c->constrainPosition();
    }
}

void CollisionScene::debugDraw()
{
    for (auto *c : activeColliders)
    {
        Debug::drawBox(c->boundingBox);
    }
}

void CollisionScene::runCollision(int *stunnedIds)
{
    // === Sweep and Prune === //
    int edgeCount = activeColliders.size() * 2;
    std::vector<ColliderEdge> colliderEdges(edgeCount);
    ColliderEdge *currEdge = &colliderEdges[0];

    // Prune along x axis by looking at min and max x of each BB
    for (size_t i = 0; i < activeColliders.size(); i++)
    {
        currEdge->isStartEdge = 1;
        currEdge->objectIndex = i;
        currEdge->x = (short)(activeColliders[i]->boundingBox.min.x);

        currEdge += 1;

        currEdge->isStartEdge = 0;
        currEdge->objectIndex = i;
        currEdge->x = (short)(activeColliders[i]->boundingBox.max.x);

        currEdge += 1;
    }

    // Sort by x position of each edge
    std::sort(colliderEdges.begin(), colliderEdges.end());

    std::vector<uint16_t> activeObjects(activeColliders.size());
    int activeObjectCount = 0;

    for (int edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++)
    {
        ColliderEdge edge = colliderEdges[edgeIndex];

        if (edge.isStartEdge)
        {
            Collider *a = activeColliders[edge.objectIndex];
            for (int activeIndex = 0; activeIndex < activeObjectCount; activeIndex++)
            {
                Collider *b = activeColliders[activeObjects[activeIndex]];

                // === AABB === //
                if (Box3D::hasOverlap(a->boundingBox, b->boundingBox))
                {
                    // === Check Collider Shapes == //
                    collide(a, b, stunnedIds);
                }
            }

            activeObjects[activeObjectCount] = edge.objectIndex;
            activeObjectCount++;
        }
        else
        {
            // End edge, remove active object
            int foundIndex = -1;

            for (int activeIndex = 0; activeIndex < activeObjectCount; activeIndex++)
            {
                if (activeObjects[activeIndex] == edge.objectIndex)
                {
                    foundIndex = activeIndex;
                    break;
                }
            }

            assert(foundIndex != -1);

            // Remove item by replacing with the last one
            activeObjects[foundIndex] = activeObjects[activeObjectCount - 1];
            activeObjectCount--;
        }
    }
}

void CollisionScene::collide(Collider *a, Collider *b, int *stunnedIds)
{
    if (!(a->collisionLayers & b->collisionLayers))
    {
        return;
    }

    if (a->collisionGroup && a->collisionGroup == b->collisionGroup)
    {
        return;
    }

    if (a->isTrigger && b->isTrigger)
    {
        return;
    }

    Simplex simplex{};
    if (!GJK::checkForOverlap(&simplex, a, b, &Vec3Right))
    {
        return;
    }

    if (a->isTrigger || b->isTrigger)
    {
        for (int i = 0; i < 4; i++)
        {
            if (stunnedIds[i] == -1)
            {
                stunnedIds[i] = a->isTrigger ? b->entityId : a->entityId;
                break;
            }
        }
        return;
    }

    EpaResult result;
    if (!EpaResult::solve(&simplex, a, b, &result))
    {
        return;
    }

    float friction = a->type.friction < b->type.friction ? a->type.friction : b->type.friction;
    float bounce = a->type.bounce > b->type.bounce ? a->type.bounce : b->type.bounce;

    correctOverlap(b, &result, -0.6f, friction, bounce);
    correctOverlap(a, &result, 0.6f, friction, bounce);
}

void CollisionScene::correctOverlap(Collider *object, EpaResult *result, float ratio, float friction, float bounce)
{
    Vector3 position = object->actor->getPosition();
    Vector3::addScaled(&position, &result->normal, result->penetration * ratio, &position);
    object->actor->setPosition(position);

    correctVelocity(object, result, ratio, friction, bounce);
}

void CollisionScene::correctVelocity(Collider *object, EpaResult *result, float ratio, float friction, float bounce)
{
    Vector3 velocity = object->actor->getVelocity();
    float velocityDot = Vector3::dot(&velocity, &result->normal);

    if ((velocityDot < 0) == (ratio < 0))
    {
        struct Vector3 tangentVelocity;

        Vector3::addScaled(&velocity, &result->normal, -velocityDot, &tangentVelocity);
        Vector3::scale(&tangentVelocity, 1.0f - friction, &tangentVelocity);
        Vector3::addScaled(&tangentVelocity, &result->normal, velocityDot * -bounce, &velocity);
        object->actor->setVelocity(velocity);
    }
}

void CollisionScene::constrainToWorld(Collider *object)
{
    if (object->isTrigger)
    {
        return;
    }

    if (object->actor->getPosition().y < 0.0f)
    {
        EpaResult result;

        Vector3 position = object->actor->getPosition();
        result.contactA = position;
        result.contactB = position;
        result.contactB.y = 0.0f;
        result.normal = Vec3Up;
        result.penetration = position.y;
        correctOverlap(object, &result, -1.0f, object->type.friction, object->type.bounce);
    }
}
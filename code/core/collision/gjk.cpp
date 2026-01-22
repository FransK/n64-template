#include "gjk.h"

using namespace Collision;

Vector3 *Simplex::addPoint(Vector3 *aPoint, Vector3 *bPoint)
{
    if (nPoints == MaxSimplexSize)
    {
        // SHOULD never happen, but just in case
        return 0;
    }

    int index = nPoints;

    objectAPoint[index] = *aPoint;
    Vector3::sub(&objectAPoint[index], bPoint, &points[index]);
    ++nPoints;

    return &points[index];
}

int Simplex::check(Vector3 *nextDirection)
{
    Vector3 *lastAdded = &points[nPoints - 1];
    Vector3 aToOrigin;
    Vector3::negate(lastAdded, &aToOrigin);

    if (nPoints == 2)
    {
        Vector3 lastAddedToOther;
        Vector3::sub(&points[0], lastAdded, &lastAddedToOther);
        Vector3::tripleProduct(&lastAddedToOther, &aToOrigin, &lastAddedToOther, nextDirection);

        if (Vector3::magSqrd(nextDirection) <= 0.0000001f)
        {
            Vector3::perp(&lastAddedToOther, nextDirection);
        }

        return 0;
    }
    else if (nPoints == 3)
    {
        Vector3 normal;
        Vector3 ab;
        Vector3::sub(&points[1], lastAdded, &ab);
        Vector3 ac;
        Vector3::sub(&points[0], lastAdded, &ac);

        Vector3::cross(&ab, &ac, &normal);

        Vector3 dirCheck;
        Vector3::cross(&ab, &normal, &dirCheck);

        if (Vector3::dot(&dirCheck, &aToOrigin) > 0.0f)
        {
            Vector3::tripleProduct(&ab, &aToOrigin, &ab, nextDirection);

            if (Vector3::magSqrd(nextDirection) <= 0.0000001f)
            {
                *nextDirection = normal;
            }

            // remove c
            movePoint(0, 1);
            movePoint(1, 2);

            nPoints = 2;

            return 0;
        }

        Vector3::cross(&normal, &ac, &dirCheck);

        if (Vector3::dot(&dirCheck, &aToOrigin) > 0.0f)
        {
            Vector3::tripleProduct(&ac, &aToOrigin, &ac, nextDirection);

            if (Vector3::magSqrd(nextDirection) <= 0.0000001f)
            {
                *nextDirection = normal;
            }

            // remove b
            movePoint(1, 2);
            nPoints = 2;

            return 0;
        }

        if (Vector3::dot(&normal, &aToOrigin) > 0.0f)
        {
            *nextDirection = normal;
            return 0;
        }

        // change triangle winding
        // temporarily use unused vertex 4
        movePoint(3, 0);
        movePoint(0, 1);
        movePoint(1, 3);
        Vector3::negate(&normal, nextDirection);
    }
    else if (nPoints == 4)
    {
        int lastBehindIndex = -1;
        int lastInFrontIndex = -1;
        int isFrontCount = 0;

        struct Vector3 normals[3];

        for (int i = 0; i < 3; ++i)
        {
            struct Vector3 firstEdge;
            struct Vector3 secondEdge;
            Vector3::sub(lastAdded, &points[i], &firstEdge);
            Vector3::sub(i == 2 ? &points[0] : &points[i + 1], &points[i], &secondEdge);
            Vector3::cross(&firstEdge, &secondEdge, &normals[i]);

            if (Vector3::dot(&aToOrigin, &normals[i]) > 0.0f)
            {
                ++isFrontCount;
                lastInFrontIndex = i;
            }
            else
            {
                lastBehindIndex = i;
            }
        }

        if (isFrontCount == 0)
        {
            // enclosed the origin
            return 1;
        }
        else if (isFrontCount == 1)
        {
            *nextDirection = normals[lastInFrontIndex];

            if (lastInFrontIndex == 1)
            {
                movePoint(0, 1);
                movePoint(1, 2);
            }
            else if (lastInFrontIndex == 2)
            {
                movePoint(1, 0);
                movePoint(0, 2);
            }

            movePoint(2, 3);
            nPoints = 3;
        }
        else if (isFrontCount == 2)
        {
            if (lastBehindIndex == 0)
            {
                movePoint(0, 2);
            }
            else if (lastBehindIndex == 2)
            {
                movePoint(0, 1);
            }

            movePoint(1, 3);
            nPoints = 2;

            struct Vector3 ab;
            Vector3::sub(&points[0], &points[1], &ab);

            Vector3::tripleProduct(&ab, &aToOrigin, &ab, nextDirection);

            if (Vector3::magSqrd(nextDirection) <= 0.0000001f)
            {
                Vector3::perp(&ab, nextDirection);
            }
        }
        else
        {
            // this case shouldn't happen but if it does
            // this is the correct logic
            movePoint(0, 3);
            nPoints = 1;
            *nextDirection = aToOrigin;
        }
    }

    return 0;
}

void Simplex::movePoint(int to, int from)
{
    points[to] = points[from];
    objectAPoint[to] = objectAPoint[from];
}

int GJK::checkForOverlap(Simplex *simplex, Collider *a, Collider *b, const Vector3 *firstDirection)
{
    Vector3 aPoint;
    Vector3 bPoint;
    Vector3 nextDirection;

    if (Vector3::isZero(firstDirection))
    {
        a->minkowskiSumWorld(&Math::Vec3Right, &aPoint);
        Vector3::negate(&Math::Vec3Right, &nextDirection);

        b->minkowskiSumWorld(&nextDirection, &bPoint);
        simplex->addPoint(&aPoint, &bPoint);
    }
    else
    {
        a->minkowskiSumWorld(firstDirection, &aPoint);
        Vector3::negate(firstDirection, &nextDirection);

        b->minkowskiSumWorld(&nextDirection, &bPoint);
        simplex->addPoint(&aPoint, &bPoint);
    }

    for (int iteration = 0; iteration < MaxGJKIterations; ++iteration)
    {
        Vector3 reverseDirection;
        Vector3::negate(&nextDirection, &reverseDirection);
        a->minkowskiSumWorld(&nextDirection, &aPoint);
        b->minkowskiSumWorld(&reverseDirection, &bPoint);

        struct Vector3 *addedPoint = simplex->addPoint(&aPoint, &bPoint);

        if (!addedPoint) // Too many points
        {
            return 0;
        }

        if (Vector3::dot(addedPoint, &nextDirection) <= 0.0f)
        {
            return 0;
        }

        if (simplex->check(&nextDirection))
        {
            return 1;
        }
    }

    return 0;
}

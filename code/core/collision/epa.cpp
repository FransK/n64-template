#include <cassert>

#include "epa.h"

#include "math/plane.h"

using namespace Collision;

void ExpandingSimplex::triangleInit(SimplexTriangle *triangle, const SimplexTriangleIndexData *data)
{
    triangle->indexData = *data;
    triangleInitNormal(triangle);
}

bool ExpandingSimplex::triangleCheckEdge(SimplexTriangle *triangle, int index)
{
    const Vector3 *pointA = &points[triangle->indexData.indices[index]];

    struct Vector3 edge;
    Vector3::sub(&points[triangle->indexData.indices[NextFace(index)]], pointA, &edge);
    struct Vector3 toOrigin;
    Vector3::negate(pointA, &toOrigin);

    struct Vector3 crossCheck;
    Vector3::cross(&edge, &toOrigin, &crossCheck);

    // check if origin is off to the side of edge
    if (Vector3::dot(&crossCheck, &triangle->normal) >= 0.0f)
    {
        return false;
    }

    float edgeLerp = Vector3::dot(&toOrigin, &edge);
    float edgeMagSqrd = Vector3::magSqrd(&edge);

    if (edgeLerp < 0.0f)
    {
        edgeLerp = 0.0f;
    }
    else if (edgeLerp > edgeMagSqrd)
    {
        edgeLerp = 1.0f;
    }
    else
    {
        edgeLerp /= edgeMagSqrd;
    }

    struct Vector3 nearestPoint;
    Vector3::addScaled(pointA, &edge, edgeLerp, &nearestPoint);

    triangle->distanceToOrigin = sqrtf(Vector3::magSqrd(&nearestPoint));

    return true;
}

void ExpandingSimplex::triangleInitNormal(SimplexTriangle *triangle)
{
    Vector3 edgeB;
    Vector3::sub(&points[triangle->indexData.indices[1]], &points[triangle->indexData.indices[0]], &edgeB);
    Vector3 edgeC;
    Vector3::sub(&points[triangle->indexData.indices[2]], &points[triangle->indexData.indices[0]], &edgeC);

    Vector3::cross(&edgeB, &edgeC, &triangle->normal);
}

void ExpandingSimplex::triangleDetermineDistance(SimplexTriangle *triangle)
{
    Vector3::normalize(&triangle->normal, &triangle->normal);

    for (int i = 0; i < 3; ++i)
    {
        if (triangleCheckEdge(triangle, i))
        {
            return;
        }
    }

    triangle->distanceToOrigin = Vector3::dot(&triangle->normal, &points[triangle->indexData.indices[0]]);
}

ExpandingSimplex::ExpandingSimplex(const Simplex *startingSimplex, int flags)
{
    assert(startingSimplex->nPoints == 4);

    triangleCount = 0;
    pointCount = 0;
    flags = flags;

    for (int i = 0; i < 4; ++i)
    {
        addPoint(&startingSimplex->objectAPoint[i], &startingSimplex->points[i]);
    }

    for (int i = 0; i < 4; ++i)
    {
        addTriangle(&InitialSimplexIndexData[i]);
    }
}

void ExpandingSimplex::addPoint(const Vector3 *aPoint, const Vector3 *pointDiff)
{
    aPoints[pointCount] = *aPoint;
    points[pointCount] = *pointDiff;
    ++pointCount;
}

void ExpandingSimplex::addTriangle(const SimplexTriangleIndexData *data)
{
    if (triangleCount == MaxSimplexTriangles)
    {
        return;
    }

    int result = triangleCount;
    triangleInit(&triangles[result], data);
    ++triangleCount;

    if (flags & static_cast<uint16_t>(SimplexFlags::SimplexFlagsSkipDistance))
    {
        return;
    }

    triangleDetermineDistance(&triangles[result]);

    triangleHeap[result] = result;
    siftUpHeap(result);
}

SimplexTriangle *ExpandingSimplex::closestFace()
{
    return &triangles[triangleHeap[0]];
}

void ExpandingSimplex::expand(int newPointIndex, int faceToRemoveIndex)
{
    if (newPointIndex == -1)
    {
        return;
    }

    SimplexTriangle *faceToRemove = &triangles[faceToRemoveIndex];
    SimplexTriangleIndexData existing = faceToRemove->indexData;

    unsigned char triangleIndices[3];
    triangleIndices[0] = faceToRemoveIndex;
    triangleIndices[1] = triangleCount;
    triangleIndices[2] = triangleCount + 1;

    // first connect all the adjacent face information
    for (int i = 0; i < 3; ++i)
    {
        SimplexTriangleIndexData next;
        int nextFace = NextFace(i);
        int nextNextFace = NextFace(nextFace);
        next.indices[0] = existing.indices[i];
        next.indices[1] = existing.indices[nextFace];
        next.indices[2] = newPointIndex;

        next.adjacentFaces[0] = existing.adjacentFaces[i];
        next.adjacentFaces[1] = triangleIndices[nextFace];
        next.adjacentFaces[2] = triangleIndices[nextNextFace];

        next.oppositePoints[0] = existing.oppositePoints[i];
        next.oppositePoints[1] = 1;
        next.oppositePoints[2] = 0;

        // update back reference to new triangle
        SimplexTriangle *otherTriangle = &triangles[existing.adjacentFaces[i]];
        int backReferenceIndex = NextFace(existing.oppositePoints[i]);
        otherTriangle->indexData.adjacentFaces[backReferenceIndex] = triangleIndices[i];
        otherTriangle->indexData.oppositePoints[backReferenceIndex] = 2;

        triangleInit(&triangles[triangleIndices[i]], &next);
    }

    // then check for edge rotation
    for (int i = 0; i < 3; ++i)
    {
        int triangleIndex = triangleIndices[i];

        if (i != 0)
        {
            triangleHeap[triangleIndex] = triangleIndex;
            ++triangleCount;
        }

        triangleCheckRotate(triangleIndex, i == 0 ? 0 : triangleIndex);
    }
}

int ExpandingSimplex::findHeapIndex(int value)
{
    for (int i = 0; i < triangleCount; ++i)
    {
        if (triangleHeap[i] == value)
        {
            return i;
        }
    }

    return 0;
}

void ExpandingSimplex::fixHeap(int heapIndex)
{
    int nextHeapIndex = siftDownHeap(heapIndex);

    if (nextHeapIndex != heapIndex)
    {
        return;
    }

    siftUpHeap(nextHeapIndex);
}

void ExpandingSimplex::rotateEdge(SimplexTriangle *triangleA, int triangleAIndex, int heapIndex)
{
    // new triangles are setup so the edge to rotate is the first edge
    int triangleBIndex = triangleA->indexData.adjacentFaces[0];

    struct SimplexTriangle *triangleB = &triangles[triangleBIndex];

    int relativeIndex0 = triangleA->indexData.oppositePoints[0];
    int relativeIndex1 = NextFace(relativeIndex0);
    int relativeIndex2 = NextFace(relativeIndex1);

    triangleA->indexData.adjacentFaces[0] = triangleB->indexData.adjacentFaces[relativeIndex2];
    triangleB->indexData.adjacentFaces[relativeIndex1] = triangleA->indexData.adjacentFaces[1];
    triangleA->indexData.adjacentFaces[1] = triangleBIndex;
    triangleB->indexData.adjacentFaces[relativeIndex2] = triangleAIndex;

    triangleA->indexData.indices[1] = triangleB->indexData.indices[relativeIndex0];
    triangleB->indexData.indices[relativeIndex2] = triangleA->indexData.indices[2];

    triangleA->indexData.oppositePoints[0] = triangleB->indexData.oppositePoints[relativeIndex2];
    triangleB->indexData.oppositePoints[relativeIndex1] = triangleA->indexData.oppositePoints[1];
    triangleA->indexData.oppositePoints[1] = relativeIndex1;
    triangleB->indexData.oppositePoints[relativeIndex2] = 0;

    // update back references from adjacent triangles
    struct SimplexTriangle *adjTriangle = &triangles[triangleA->indexData.adjacentFaces[0]];
    int adjIndex = NextFace(triangleA->indexData.oppositePoints[0]);
    adjTriangle->indexData.adjacentFaces[adjIndex] = triangleAIndex;
    adjTriangle->indexData.oppositePoints[adjIndex] = 2;

    adjTriangle = &triangles[triangleB->indexData.adjacentFaces[relativeIndex1]];
    adjIndex = NextFace(triangleB->indexData.oppositePoints[relativeIndex1]);
    adjTriangle->indexData.adjacentFaces[adjIndex] = triangleBIndex;
    adjTriangle->indexData.oppositePoints[adjIndex] = relativeIndex0;

    triangleInitNormal(triangleA);
    if (!(flags & static_cast<uint16_t>(SimplexFlags::SimplexFlagsSkipDistance)))
    {
        triangleDetermineDistance(triangleA);
        fixHeap(heapIndex);
    }

    triangleInitNormal(triangleB);
    if (!(flags & static_cast<uint16_t>(SimplexFlags::SimplexFlagsSkipDistance)))
    {
        triangleDetermineDistance(triangleB);
        fixHeap(findHeapIndex(triangleBIndex));
    }
}

int ExpandingSimplex::siftUpHeap(int heapIndex)
{
    int parentHeapIndex = GetParentIndex(heapIndex);
    float currentDistance = ExpandingSimplexGetDistance(this, triangleHeap[heapIndex]);

    while (heapIndex > 0)
    {
        // already heaped
        if (currentDistance >= ExpandingSimplexGetDistance(this, triangleHeap[parentHeapIndex]))
        {
            break;
        }

        // swap the parent with the current node
        int tmp = triangleHeap[heapIndex];
        triangleHeap[heapIndex] = triangleHeap[parentHeapIndex];
        triangleHeap[parentHeapIndex] = tmp;

        // move up to the parent
        heapIndex = parentHeapIndex;
        parentHeapIndex = GetParentIndex(heapIndex);
    }

    return heapIndex;
}

int ExpandingSimplex::siftDownHeap(int heapIndex)
{
    float currentDistance = ExpandingSimplexGetDistance(this, triangleHeap[heapIndex]);

    while (heapIndex < triangleCount)
    {
        int swapWithChild = -1;

        int childHeapIndex = GetChildIndex(heapIndex, 0);

        // reached the end of the heap
        if (childHeapIndex >= triangleCount)
        {
            break;
        }

        float childDistance = ExpandingSimplexGetDistance(this, triangleHeap[childHeapIndex]);

        // check that we don't run off the end of the heap
        if (childDistance < currentDistance)
        {
            swapWithChild = childHeapIndex;
        }

        float otherChildDistance = ExpandingSimplexGetDistance(this, triangleHeap[childHeapIndex + 1]);

        // grab the smallest child
        if (childHeapIndex + 1 < triangleCount && otherChildDistance < currentDistance && otherChildDistance < childDistance)
        {
            swapWithChild = childHeapIndex + 1;
        }

        if (swapWithChild == -1)
        {
            // no child out of order
            break;
        }

        // swap child with the current node
        int tmp = triangleHeap[heapIndex];
        triangleHeap[heapIndex] = triangleHeap[swapWithChild];
        triangleHeap[swapWithChild] = tmp;

        heapIndex = swapWithChild;
    }

    return heapIndex;
}

void ExpandingSimplex::triangleCheckRotate(int triangleIndex, int heapIndex)
{
    struct SimplexTriangle *triangle = &triangles[triangleIndex];
    struct SimplexTriangle *adjacent = &triangles[triangle->indexData.adjacentFaces[0]];
    struct Vector3 *oppositePoint = &points[adjacent->indexData.indices[triangle->indexData.oppositePoints[0]]];

    struct Vector3 *firstPoint = &points[triangle->indexData.indices[0]];

    struct Vector3 offset;
    Vector3::sub(oppositePoint, firstPoint, &offset);

    if (Vector3::dot(&offset, &triangle->normal) > 0.0f)
    {
        rotateEdge(triangle, triangleIndex, heapIndex);
    }
    else if (!(flags & static_cast<uint16_t>(SimplexFlags::SimplexFlagsSkipDistance)))
    {
        triangleDetermineDistance(triangle);
        fixHeap(heapIndex);
    }
}

void EpaResult::calculateContact(struct ExpandingSimplex *simplex, struct SimplexTriangle *closestFace, struct Vector3 *planePos, struct EpaResult *result)
{
    struct Vector3 baryCoords;

    struct Vector3 *a = &simplex->points[closestFace->indexData.indices[0]];
    struct Vector3 *b = &simplex->points[closestFace->indexData.indices[1]];
    struct Vector3 *c = &simplex->points[closestFace->indexData.indices[2]];

    Plane::calculateBarycentricCoords(
        a,
        b,
        c,
        planePos,
        &baryCoords);

    Plane::evaluateBarycentricCoords(
        a,
        b,
        c,
        &baryCoords,
        &result->contactA);

    Vector3::addScaled(&result->contactA, &result->normal, result->penetration, &result->contactB);
}

bool EpaResult::solve(const Simplex *startingSimplex, Collider *a, Collider *b, EpaResult *out)
{
    ExpandingSimplex simplex(startingSimplex, 0);
    struct SimplexTriangle *closestFace = 0;
    float projection = 0.0f;

    for (int i = 0; i < MaxIterations; ++i)
    {
        struct Vector3 reverseNormal;

        closestFace = simplex.closestFace();

        int nextIndex = simplex.pointCount;

        struct Vector3 *aPoint = &simplex.aPoints[nextIndex];
        struct Vector3 bPoint;

        a->minkowskiSumWorld(&closestFace->normal, aPoint);
        Vector3::negate(&closestFace->normal, &reverseNormal);
        b->minkowskiSumWorld(&reverseNormal, &bPoint);

        Vector3::sub(aPoint, &bPoint, &simplex.points[nextIndex]);

        projection = Vector3::dot(&simplex.points[nextIndex], &closestFace->normal);

        if ((projection - closestFace->distanceToOrigin) < 0.001f)
        {
            break;
        }

        ++simplex.pointCount;
        simplex.expand(nextIndex, simplex.triangleHeap[0]);
    }

    if (closestFace && Vector3::magSqrd(&closestFace->normal) > 0.5f)
    {
        out->normal = closestFace->normal;
        out->penetration = -projection;
        struct Vector3 planePos;
        Vector3::scale(&closestFace->normal, closestFace->distanceToOrigin, &planePos);
        calculateContact(&simplex, closestFace, &planePos, out);

        return true;
    }

    return false;
}
#pragma once

#include <cstdint>

#include "math/vector3.h"

#include "collider.h"
#include "gjk.h"

using namespace Math;

namespace Collision
{
    constexpr uint16_t MaxIterations = 10;
    constexpr uint16_t MaxSimplexPoints = 4 + MaxIterations;
    constexpr uint16_t MaxSimplexTriangles = 4 + MaxIterations * 2;

    constexpr uint16_t NextFace(uint16_t index)
    {
        return index == 2 ? 0 : index + 1;
    }

    constexpr int GetChildIndex(int heapIndex, int childHeapIndex)
    {
        return ((heapIndex) << 1) + 1 + childHeapIndex;
    }

    constexpr int GetParentIndex(int heapIndex)
    {
        return ((heapIndex - 1) >> 1);
    }

    union SimplexTriangleIndexData
    {
        struct
        {
            unsigned char indices[3];
            unsigned char adjacentFaces[3];
            unsigned char oppositePoints[3];
        };
        int alignment;
    };

    constexpr SimplexTriangleIndexData InitialSimplexIndexData[] = {
        {{{0, 1, 2}, {3, 1, 2}, {2, 2, 2}}},
        {{{2, 1, 3}, {0, 3, 2}, {0, 1, 0}}},
        {{{0, 2, 3}, {0, 1, 3}, {1, 1, 0}}},
        {{{1, 0, 3}, {0, 2, 1}, {2, 1, 0}}},
    };

    struct SimplexTriangle
    {
        SimplexTriangleIndexData indexData{};
        float distanceToOrigin{};
        Vector3 normal{};
    };

    enum struct SimplexFlags
    {
        SimplexFlagsSkipDistance = (1 << 0),
    };

    struct ExpandingSimplex
    {
        Vector3 points[MaxSimplexPoints];
        Vector3 aPoints[MaxSimplexPoints];
        SimplexTriangle triangles[MaxSimplexTriangles];
        short pointCount;
        short triangleCount;
        unsigned char triangleHeap[MaxSimplexTriangles];
        short flags;

        ExpandingSimplex(const Simplex *startingSimplex, int flags);

        void addPoint(const Vector3 *aPoint, const Vector3 *pointDiff);
        void addTriangle(const SimplexTriangleIndexData *data);
        SimplexTriangle *closestFace();
        void expand(int newPointIndex, int faceToRemoveIndex);
        int findHeapIndex(int value);
        void fixHeap(int heapIndex);
        void rotateEdge(SimplexTriangle *triangleA, int triangleAIndex, int heapIndex);
        int siftDownHeap(int heapIndex);
        int siftUpHeap(int heapIndex);
        void triangleCheckRotate(int triangleIndex, int heapIndex);
        void triangleInit(SimplexTriangle *triangle, const SimplexTriangleIndexData *indexData);
        bool triangleCheckEdge(SimplexTriangle *triangle, int index);
        void triangleDetermineDistance(SimplexTriangle *triangle);
        void triangleInitNormal(SimplexTriangle *triangle);
    };

    constexpr float ExpandingSimplexGetDistance(const ExpandingSimplex *simplex, int triangleIndex)
    {
        return simplex->triangles[triangleIndex].distanceToOrigin;
    }

    struct EpaResult
    {
        Vector3 contactA;
        Vector3 contactB;
        Vector3 normal; // points from A to B
        float penetration;

        static bool solve(const Simplex *startingSimplex, Collider *a, Collider *b, EpaResult *out);
        static void calculateContact(struct ExpandingSimplex *simplex, struct SimplexTriangle *closestFace, struct Vector3 *planePos, struct EpaResult *result);
    };
}
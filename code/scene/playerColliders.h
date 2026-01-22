#pragma once

#include "collision/colliderType.h"
#include "collision/cylinder.h"
#include "collision/sphere.h"

constexpr Collision::ColliderType PlayerColliderType = {
    .minkowskiSum = Collision::Cylinder::MinkowskiSum,
    .boundingBoxCalculator = Collision::Cylinder::BoundingBox,
    .data = {
        .cylinder = {
            .radius = 5.0f,
            .halfHeight = 12.0f}},
    .bounce = 0.0f,
    .friction = 0.0f};

constexpr Collision::ColliderType DamageTriggerType = {
    .minkowskiSum = Collision::Sphere::MinkowskiSum,
    .boundingBoxCalculator = Collision::Sphere::BoundingBox,
    .data = {
        .sphere = {
            .radius = 4.0f}},
    .bounce = 0.0f,
    .friction = 0.0f};
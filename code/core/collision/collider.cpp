#include "collider.h"

using namespace Collision;

void Collider::update(float timeStep)
{
    if (hasGravity)
    {
        float y = actor->getVelocity().y;
        y += timeStep * GRAVITY_CONSTANT;
        actor->setVelocity({actor->getVelocity().x, y, actor->getVelocity().z});
    }

    Vector3 position = actor->getPosition();
    Vector3 velocity = actor->getVelocity();
    Vector3::addScaled(&position, &velocity, timeStep, &position);
    actor->setPosition(position);
}

void Collider::recalcBB()
{
    Vector3 position = actor->getPosition();
    Vector2 rotation = actor->getRotation();
    type.boundingBoxCalculator(&type.data, &rotation, &boundingBox);
    Vector3 offset;
    if (scale != 1.0f)
    {
        Vector3::scale(&boundingBox.min, scale, &boundingBox.min);
        Vector3::scale(&boundingBox.max, scale, &boundingBox.max);
        Vector3::addScaled(&position, &center, scale, &offset);
    }
    else
    {
        Vector3::add(&center, &position, &offset);
    }
    Vector3::add(&boundingBox.min, &offset, &boundingBox.min);
    Vector3::add(&boundingBox.max, &offset, &boundingBox.max);
}

void Collider::constrainPosition()
{
    Vector3 position = actor->getPosition();
    float squared_position = position.x * position.x + position.z * position.z;

    if (squared_position > PLAYING_R2)
    {
        Vector3::normAndScale(&position, PLAYING_R, &position);
        actor->setPosition(position);
    }
}

void Collider::minkowskiSumWorld(const Vector3 *direction, Vector3 *output)
{
    type.minkowskiSum(&type.data, direction, output);

    if (scale != 1.0f)
    {
        Vector3::scale(output, scale, output);
        Vector3::addScaled(output, &center, scale, output);
    }
    else
    {
        Vector3::add(output, &center, output);
    }

    Vector3 position = actor->getPosition();
    Vector3::add(output, &position, output);
}
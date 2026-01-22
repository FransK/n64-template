#ifndef CORE_SCENE_ENTITY_H
#define CORE_SCENE_ENTITY_H

#include "vector2.h"
#include "vector3.h"

class Entity
{
public:
    [[nodiscard]] Math::Vector3 getPosition() const { return mPosition; }
    [[nodiscard]] Math::Vector2 getRotation() const { return mRotation; }

    void setPosition(const Math::Vector3 &pos) { mPosition = pos; }
    void setRotation(const Math::Vector2 &rot) { mRotation = rot; }

private:
    Math::Vector3 mPosition{};
    Math::Vector2 mRotation{};
};

#endif
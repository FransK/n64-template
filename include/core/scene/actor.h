#ifndef CORE_SCENE_ACTOR_H
#define CORE_SCENE_ACTOR_H

#include "entity.h"
#include "vector3.h"

class Actor : public Entity
{
public:
    [[nodiscard]] Math::Vector3 getVelocity() const { return mVelocity; }

    void setVelocity(const Math::Vector3 &vel) { mVelocity = vel; }

private:
    Math::Vector3 mVelocity{};
};

#endif
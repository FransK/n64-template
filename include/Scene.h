#ifndef N64_SCENE_H
#define N64_SCENE_H

#include "ComponentRegistry.h"

namespace n64
{
    class Scene
    {
    public:
        void updateFixed(float deltaTime);
        void update(float deltaTime);

    private:
        ecs::ComponentRegistry mComponentRegistry{};
    };
}
#endif // N64_SCENE_H
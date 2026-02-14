#ifndef N64_ECS_COMPONENT_REGISTRY_H
#define N64_ECS_COMPONENT_REGISTRY_H

#include <any>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "ComponentStore.h"
#include "EntityId.h"

namespace n64::ecs
{
    class Registry
    {
    public:
        template <typename EntityIdType, typename... Components>
        std::vector<std::tuple<EntityIdType, Components...>> components()
        {
            std::vector<std::tuple<EntityIdType, Components...>> result{};

            // 1. Get set of EntityIds that have ALL requested components
            auto &firstStore = getStore<std::tuple_element_t<0, std::tuple<Components...>>>();

            for (const auto &entityId : firstStore.entityList())
            {
                // Check all stores have this entity
                bool allPresent = (... && (getStore<Components>().has(entityId)));
                if (allPresent)
                {
                    result.emplace_back({entityId, getStore<Components>().get(entityId)...});
                }
            }

            return result;
        }

        template <typename ComponentType>
        void addComponent(EntityId entityId, ComponentType component)
        {
            getStore<ComponentType>().addComponent(entityId, std::move(component));
        }

        template <typename... Components>
        void updateComponent(EntityId entityId, Components... components)
        {
            (getStore<Components>().update(entityId, std::move(components)), ...);
        }

    private:
        std::unordered_map<std::type_index, std::any> mStores;

        template <typename ComponentType>
        ComponentStore<ComponentType> &getStore()
        {
            auto key = std::type_index(typeid(ComponentType));
            if (mStores.find(key) == mStores.end())
            {
                mStores[key] = ComponentStore<ComponentType>{};
            }
            return std::any_cast<ComponentStore<ComponentType> &>(mStores[key]);
        }
    };
}

#endif // N64_ECS_COMPONENT_REGISTRY_H
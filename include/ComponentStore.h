#ifndef N64_ECS_COMPONENT_STORE_H
#define N64_ECS_COMPONENT_STORE_H

#include <map>
#include <vector>
#include "EntityId.h"
#include "Logger.h"

namespace n64::ecs
{
    template <typename T>
    class ComponentStore
    {
    public:
        void addComponent(EntityId entityId, T component)
        {
            if (has(entityId))
            {
                Logger::error("ComponentStore", "Store already has entity ID: " + std::to_string(entityId) + ". Did you mean to call update()?");
                return;
            }
            mIdComponentMap[entityId] = std::move(component);
        }

        T get(EntityId entityId)
        {
            return mIdComponentMap.at(entityId);
        }

        void update(EntityId entityId, T component)
        {
            if (!has(entityId))
            {
                Logger::error("ComponentStore", "Failed to update component for entity ID: " + entityId);
                return;
            }
            mIdComponentMap[entityId] = component;
        }

        bool has(EntityId entityId) const
        {
            return mIdComponentMap.count(entityId) > 0;
        }

        std::vector<EntityId> entityList() const
        {
            std::vector<EntityId> ids;
            ids.reserve(mIdComponentMap.size());
            for (const auto &[id, _] : mIdComponentMap)
            {
                ids.push_back(id);
            }
            return ids;
        }

    private:
        std::map<EntityId, T> mIdComponentMap{};
    };
}

#endif // N64_ECS_COMPONENT_STORE_H
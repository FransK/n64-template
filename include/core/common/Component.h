#ifndef CORE_COMMON_COMPONENT_H
#define CORE_COMMON_COMPONENT_H

#include <memory>

// A type-erased component that can hold any type T that has an update function defined.
class Component
{
public:
    template <typename T>
    Component(T x) : self_(std::make_unique<ComponentModel<T>>(std::move(x))) {}

    friend void update(Component &c, float deltaTime)
    {
        c.self_->update_(deltaTime);
    }

private:
    struct ComponentConcept
    {
        virtual ~ComponentConcept() = default;
        virtual void update_(float deltaTime) = 0;
    };

    template <typename T>
    struct ComponentModel final : ComponentConcept
    {
        ComponentModel(T x) : data_(std::move(x)) {}
        void update_(float deltaTime) override { update(data_, deltaTime); }

        T data_;
    };

    std::unique_ptr<ComponentConcept> self_;
};

#endif // CORE_COMMON_COMPONENT_H
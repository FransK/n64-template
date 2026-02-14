#include <catch2/catch_test_macros.hpp>
#include "ComponentStore.h"

using n64::EntityId;
using n64::ecs::ComponentStore;

struct Position
{
    float x;
    float y;
};

struct Velocity
{
    float dx;
    float dy;
};

TEST_CASE("ComponentStore addComponent and get", "[ComponentStore]")
{
    ComponentStore<Position> store;
    EntityId id = 1;

    store.addComponent(id, Position{10.0f, 20.0f});

    auto pos = store.get(id);
    REQUIRE(pos.x == 10.0f);
    REQUIRE(pos.y == 20.0f);
}

TEST_CASE("ComponentStore has", "[ComponentStore]")
{
    ComponentStore<Position> store;
    EntityId id = 1;

    REQUIRE_FALSE(store.has(id));

    store.addComponent(id, Position{0.0f, 0.0f});

    REQUIRE(store.has(id));
}

TEST_CASE("ComponentStore update", "[ComponentStore]")
{
    ComponentStore<Position> store;
    EntityId id = 1;

    store.addComponent(id, Position{1.0f, 2.0f});
    store.update(id, Position{3.0f, 4.0f});

    auto pos = store.get(id);
    REQUIRE(pos.x == 3.0f);
    REQUIRE(pos.y == 4.0f);
}

TEST_CASE("ComponentStore update on missing entity logs error", "[ComponentStore]")
{
    ComponentStore<Position> store;
    EntityId id = 99;

    // Should not throw, just log an error
    REQUIRE_NOTHROW(store.update(id, Position{1.0f, 1.0f}));
    REQUIRE_FALSE(store.has(id));
}

TEST_CASE("ComponentStore entityList", "[ComponentStore]")
{
    ComponentStore<Velocity> store;

    store.addComponent(1, Velocity{1.0f, 0.0f});
    store.addComponent(2, Velocity{0.0f, 1.0f});
    store.addComponent(3, Velocity{1.0f, 1.0f});

    auto ids = store.entityList();
    REQUIRE(ids.size() == 3);

    // std::map keeps keys sorted, so order is deterministic
    REQUIRE(ids[0] == 1);
    REQUIRE(ids[1] == 2);
    REQUIRE(ids[2] == 3);
}

TEST_CASE("ComponentStore addComponent on existing logs error", "[ComponentStore]")
{
    ComponentStore<Position> store;
    EntityId id = 1;

    store.addComponent(id, Position{1.0f, 2.0f});

    REQUIRE_NOTHROW(store.addComponent(id, Position{5.0f, 6.0f}));
    auto pos = store.get(id);
    REQUIRE(pos.x == 1.0f);
    REQUIRE(pos.y == 2.0f);

    // Should still only have one entity
    REQUIRE(store.entityList().size() == 1);
}

TEST_CASE("ComponentStore works with multiple entities", "[ComponentStore]")
{
    ComponentStore<Position> store;

    store.addComponent(10, Position{1.0f, 2.0f});
    store.addComponent(20, Position{3.0f, 4.0f});
    store.addComponent(30, Position{5.0f, 6.0f});

    REQUIRE(store.get(10).x == 1.0f);
    REQUIRE(store.get(20).x == 3.0f);
    REQUIRE(store.get(30).x == 5.0f);

    REQUIRE(store.has(10));
    REQUIRE(store.has(20));
    REQUIRE(store.has(30));
    REQUIRE_FALSE(store.has(40));
}

#include <catch2/catch_test_macros.hpp>
#include "ComponentRegistry.h"

using n64::EntityId;
using n64::ecs::ComponentRegistry;

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

struct Health
{
    int hp;
};

TEST_CASE("ComponentRegistry addComponent and query single component", "[ComponentRegistry]")
{
    ComponentRegistry registry;

    registry.addComponent<Position>(1, {10.0f, 20.0f});
    registry.addComponent<Position>(2, {30.0f, 40.0f});

    auto results = registry.components<Position>();
    REQUIRE(results.size() == 2);

    auto [id1, pos1] = results[0];
    REQUIRE(id1 == 1);
    REQUIRE(pos1.x == 10.0f);

    auto [id2, pos2] = results[1];
    REQUIRE(id2 == 2);
    REQUIRE(pos2.x == 30.0f);
}

TEST_CASE("ComponentRegistry query multiple components", "[ComponentRegistry]")
{
    ComponentRegistry registry;

    registry.addComponent<Position>(1, {1.0f, 2.0f});
    registry.addComponent<Velocity>(1, {3.0f, 4.0f});

    registry.addComponent<Position>(2, {5.0f, 6.0f});
    // Entity 2 has no Velocity

    auto results = registry.components<Position, Velocity>();
    REQUIRE(results.size() == 1);

    auto [id, pos, vel] = results[0];
    REQUIRE(id == 1);
    REQUIRE(pos.x == 1.0f);
    REQUIRE(vel.dx == 3.0f);
}

TEST_CASE("ComponentRegistry query returns empty when no entities match", "[ComponentRegistry]")
{
    ComponentRegistry registry;

    registry.addComponent<Position>(1, {1.0f, 2.0f});

    auto results = registry.components<Velocity>();
    REQUIRE(results.empty());
}

TEST_CASE("ComponentRegistry updateComponent single component", "[ComponentRegistry]")
{
    ComponentRegistry registry;

    registry.addComponent<Position>(1, {1.0f, 2.0f});
    registry.updateComponent(1, Position{10.0f, 20.0f});

    auto results = registry.components<Position>();
    REQUIRE(results.size() == 1);

    auto [id, pos] = results[0];
    REQUIRE(pos.x == 10.0f);
    REQUIRE(pos.y == 20.0f);
}

TEST_CASE("ComponentRegistry updateComponent multiple components", "[ComponentRegistry]")
{
    ComponentRegistry registry;

    registry.addComponent<Position>(1, {1.0f, 2.0f});
    registry.addComponent<Velocity>(1, {3.0f, 4.0f});

    registry.updateComponent(1, Position{10.0f, 20.0f}, Velocity{30.0f, 40.0f});

    auto results = registry.components<Position, Velocity>();
    REQUIRE(results.size() == 1);

    auto [id, pos, vel] = results[0];
    REQUIRE(pos.x == 10.0f);
    REQUIRE(vel.dx == 30.0f);
}

TEST_CASE("ComponentRegistry updateComponent on missing entity does not add it", "[ComponentRegistry]")
{
    ComponentRegistry registry;

    REQUIRE_NOTHROW(registry.updateComponent(99, Position{1.0f, 1.0f}));

    auto results = registry.components<Position>();
    REQUIRE(results.empty());
}

TEST_CASE("ComponentRegistry components filters to intersection", "[ComponentRegistry]")
{
    ComponentRegistry registry;

    // Entity 1: Position + Velocity + Health
    registry.addComponent<Position>(1, {1.0f, 2.0f});
    registry.addComponent<Velocity>(1, {3.0f, 4.0f});
    registry.addComponent<Health>(1, {100});

    // Entity 2: Position + Health (no Velocity)
    registry.addComponent<Position>(2, {5.0f, 6.0f});
    registry.addComponent<Health>(2, {50});

    // Entity 3: Position only
    registry.addComponent<Position>(3, {7.0f, 8.0f});

    // Query Position + Velocity: only entity 1
    auto pv = registry.components<Position, Velocity>();
    REQUIRE(pv.size() == 1);
    REQUIRE(std::get<0>(pv[0]) == 1);

    // Query Position + Health: entities 1 and 2
    auto ph = registry.components<Position, Health>();
    REQUIRE(ph.size() == 2);

    // Query all three: only entity 1
    auto pvh = registry.components<Position, Velocity, Health>();
    REQUIRE(pvh.size() == 1);
    REQUIRE(std::get<0>(pvh[0]) == 1);
}

TEST_CASE("ComponentRegistry manages independent stores per type", "[ComponentRegistry]")
{
    ComponentRegistry registry;

    registry.addComponent<Position>(1, {1.0f, 2.0f});
    registry.addComponent<Velocity>(1, {3.0f, 4.0f});

    // Update only Position, Velocity should be unchanged
    registry.updateComponent(1, Position{99.0f, 99.0f});

    auto results = registry.components<Position, Velocity>();
    REQUIRE(results.size() == 1);

    auto [id, pos, vel] = results[0];
    REQUIRE(pos.x == 99.0f);
    REQUIRE(vel.dx == 3.0f);
    REQUIRE(vel.dy == 4.0f);
}

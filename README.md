# N64 Fishing Minigame

My first minigame using [Libdragon](https://github.com/DragonMinded/libdragon) Nintendo 64 SDK and [Tiny3D](https://github.com/HailToDodongo/tiny3d) graphics!

In this fishing game, the goal is simple: Fish the waters around the island to catch the most fish in the allotted time. Just watch out for aggressive anglers nearby!


### Running the game

In order to run the game, you will need the [N64brew GameJam](https://github.com/n64brew/N64brew-GameJam2024) loader and the most recent [N64 fishing assets](https://github.com/FransK/n64-assets).


### Implemented Features

- GJK/EPA Collision system using custom Minkowski sums and 2 pruning passes with single dimension edge checking and AABB overlap checking
- State machines for main scene and AI players
- Game logic for fishing, accruing points, and pushing other players to stun them
- Modified CC0 animations with reduced tri count to optimize performance on N64 hardware
- Coding decisions with thought to limited hardware (e.g. avoiding virtuals and std::function, instead favouring switch/case statements)


### Sample Gameplay

![Fishing gameplay demonstration](docs/fishing.gif)

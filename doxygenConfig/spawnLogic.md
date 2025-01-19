# Spawn Logic for Walls, Boss, and Enemies

This document describes how the game engine spawns and manages walls (two fixed in-game walls), the level-end boss, and regular enemies (three difficulty levels). All of these spawns are orchestrated in the **server** side of the code, primarily in the **`GameEngine`** methods.

## Table of Contents
1. [Overview of Spawning Mechanisms](#overview-of-spawning-mechanisms)
2. [Wall Spawning](#wall-spawning)
3. [Boss Spawning](#boss-spawning)
    - [When and How the Boss Appears](#when-and-how-the-boss-appears)
    - [Boss Attributes and Behaviors](#boss-attributes-and-behaviors)
4. [Enemy Spawning](#enemy-spawning)
    - [Three Enemy Levels](#three-enemy-levels)
    - [Random Spawns](#random-spawns)
    - [Shooting Variations](#shooting-variations)
5. [Code References](#code-references)

---

## Overview of Spawning Mechanisms

The **`GameEngine`** maintains a variety of ECS (Entity Component System) *systems* and *queues* to spawn or initialize entities. Each type of entity—wall, boss, regular enemies—relies on specialized logic within these methods:

- **`handleWallSpawns()`**
- **`spawnWall(float x, float y)`**
- **`handleEnemySpawns(float dt)`**
- **`spawnEnemy(float x, float y, int level, bool isBoss)`**
- **`spawnEnemiesForLevel(int level)`** (called on level transition)

### Flow
1. **Every server update** (`update()` loop in `GameEngine.cpp`), the engine calls methods like `handleWallSpawns()`, `handleEnemySpawns(dt)`, etc.
2. **When conditions are met** (e.g., time intervals, score thresholds, or manual triggers), new entities (walls, bosses, enemies) are created via `EntityManager::createEntity()`.
3. **Entity components** (like `Position`, `Velocity`, `Enemy`, `Wall`, etc.) are added, and special flags (`isBoss`) or attributes are set (e.g., HP, shoot speed).

---

## Wall Spawning

### 1. Fixed Position, No Movement
Two walls are present in the game:
- They do **not move** (velocity is set to zero).
- They stay throughout the entire match.

### 2. `handleWallSpawns()`
- Called periodically, as shown in the `update()` method.
- By default, it spawns walls at **x=250**, **y=100** and **x=250**, **y=450** (or any coordinate you define).
- The code snippet might look like:
  ```cpp
  void GameEngine::handleWallSpawns() {
      auto currentTime = std::chrono::steady_clock::now();
      float dt = std::chrono::duration<float>(currentTime - lastUpdateWallShoot).count();

      if (dt >= 1.2f) {
          lastUpdateWallShoot = currentTime;
          spawnWall(250, 100);
          spawnWall(250, 450);
      }
  }
  ```
  This example re-spawns them if needed, but usually you just spawn them **once** or at set intervals.

### 3. `spawnWall(float x, float y)`
- Creates the wall entity, assigns **`Position{ x, y }`** and a **`Wall`** component with relevant HP or collision size.
- **No velocity** is applied, so the wall remains stationary.

---

## Boss Spawning

### When and How the Boss Appears
- The boss is typically spawned **at the end of each level**. Once the player’s score or kills surpass certain thresholds, the game triggers:
  ```cpp
  if (!hasBossIsDisplay && !bossOfLevelIsDead) {
      // Clear enemy queue
      enemySpawnQueue.clear();
      // Enqueue a boss to spawn
      enemySpawnQueue.push_back(PendingSpawn{2.0f, 820.0f, 300.0f, currentLevel, true});
      hasBossIsDisplay = true;
  }
  ```
- **`isBoss = true`** indicates we’re dealing with the boss logic.

### Boss Attributes and Behaviors
1. **Higher HP**:  
   Boss life is multiplied compared to normal enemies. In `spawnEnemy()`, for example:
   ```cpp
   if (isBoss) {
       life = life * 5;     // Boss gets more HP
       damage = damage * 3; // Or any suitable factor
       // speedShoot can also be adjusted
   }
   ```
2. **Shooting Triple Missiles** in a crossing pattern:
    - In **`handleEnemyShoot()`**, if `enemy.isBoss == true`, the system calls `shoot_system_.update(...)` multiple times with different offsets:
      ```cpp
      shoot_system_.update(entities, enemy, true, 50);   // Shoot angled up
      shoot_system_.update(entities, enemy, true, 0);    // Shoot straight
      shoot_system_.update(entities, enemy, true, -50);  // Shoot angled down
      ```
   This spawns three projectiles simultaneously in different directions.
3. **Boss HP Gains for Player** (Optional Variation):
    - If you want the boss to *give HP to the player* upon defeat, see `handleCollision(...)` or `handleCollisionPlayer(...)`; you can add logic to grant HP or an item when the boss entity is destroyed.

---

## Enemy Spawning

### Three Enemy Levels
- The `enemyAttributes` map defines different stats for each “level” of enemy:
  ```cpp
  static inline const std::unordered_map<int, std::tuple<float, int, float>> enemyAttributes = {
      {1, {1, 5, 100.0f}},   // e.g., HP=1, damage=5, shoot speed=100.0f
      {2, {3, 5, 300.0f}},   // e.g., HP=3, damage=5, shoot speed=300.0f
      {3, {5, 5, 600.0f}}    // e.g., HP=5, damage=5, shoot speed=600.0f
  };
  ```
- Each entry typically corresponds to:
    1. HP or life (float)
    2. Damage (int)
    3. **speedShoot** or projectile velocity (float)

### Random Spawns
1. The server uses a **spawn queue** (`enemySpawnQueue`) to schedule enemy creation over time.
2. In `handleEnemySpawns(float dt)`, the engine decrements each entry’s `delay`. When it hits **<= 0**, it calls:
   ```cpp
   spawnEnemy(it->x, it->y, it->level, it->isBoss);
   ```
3. **Random Y** coordinate is typically assigned via `rand() % 560` or similar. X is often **820** for right-side spawns (scrolling from right to left).
4. These spawns happen continuously until the queue is empty or a new wave is triggered.

### Shooting Variations
- Basic (non-boss) enemies have:
    - Lower HP
    - Slower or single directional shooting (in `handleEnemyShoot()`, they shoot straight to the left).
- `ShootSystem::update()` references the enemy’s **`speedShoot`** for projectile velocity.
- If an enemy is `level = 1`, the speed might be 100.0f, so it fires slower bullets. Higher levels or bosses have faster bullets.

---

## Code References

Below are the key methods that control this logic:

- **`GameEngine::handleWallSpawns()`**: Spawns the two walls at a chosen interval or only once.
- **`GameEngine::spawnWall(float x, float y)`**: Creates a stationary wall entity.
- **`GameEngine::handleEnemySpawns(float dt)`**: Processes pending enemy spawns from `enemySpawnQueue`.
- **`GameEngine::spawnEnemy(float x, float y, int level, bool isBoss)`**: Creates an enemy entity with the relevant components (HP, velocity, isBoss flag).
- **`GameEngine::handleEnemyShoot()`**: Differentiates between normal enemies and bosses (triple shot).
- **`GameEngine::updatePlayerScore()`** / **`switchToNextLevel()`**: Might trigger the boss spawn or level transition logic.

Refer to [the server code](../server/game/GameEngine.cpp) for detailed implementation and do not forget to check each method’s Doxygen comments if you need more details on parameters and returns.

# How to Add New Game Content (Levels, Bosses, Textures, Audio)

This document explains how to extend and customize the R-Type server/client by adding new game content such as additional levels, new enemy (boss) types, graphics, and audio resources.

## Table of Contents
1. [Project Overview](#project-overview)
2. [Adding a New Level](#adding-a-new-level)
   - [Step 1: Update Level Spawning Logic](#step-1-update-level-spawning-logic)
   - [Step 2: Configure Enemy Waves](#step-2-configure-enemy-waves)
   - [Step 3: (Optional) Add Custom Assets for the Level](#step-3-optional-add-custom-assets-for-the-level)
3. [Adding a Boss Enemy](#adding-a-boss-enemy)
   - [Step 1: Extend Enemy Attributes](#step-1-extend-enemy-attributes)
   - [Step 2: Modify Spawning Logic](#step-2-modify-spawning-logic)
   - [Step 3: Customize Boss Mechanics](#step-3-customize-boss-mechanics)
4. [Adding or Updating Textures](#adding-or-updating-textures)
   - [Step 1: Place Textures in the Assets Folder](#step-1-place-textures-in-the-assets-folder)
   - [Step 2: Load the Texture in the Resource Manager](#step-2-load-the-texture-in-the-resource-manager)
   - [Step 3: Attach the Texture to an Entity](#step-3-attach-the-texture-to-an-entity)
5. [Adding or Updating Audio](#adding-or-updating-audio)
   - [Step 1: Place Audio Files in the Assets Folder](#step-1-place-audio-files-in-the-assets-folder)
   - [Step 2: Load Audio in Game or Resource Manager](#step-2-load-audio-in-game-or-resource-manager)
   - [Step 3: Play Audio in Code](#step-3-play-audio-in-code)
6. [Common Pitfalls and Tips](#common-pitfalls-and-tips)
7. [FAQ](#faq)
8. [Further Reading](#further-reading)

---

## Project Overview

Our R-Type project is divided into **server** (game logic) and **client** (rendering & player input) components. Below is a simplified overview:

- **Server**:
   - Maintains authority over all game logic: spawns enemies, updates game state, handles collisions and scoreboard, etc.
   - Implements the ECS logic using systems such as `MovementSystem`, `ShootSystem`, etc.
   - Stores and broadcasts world state changes to clients via the network.

- **Client**:
   - Renders game entities, updates UI elements, and handles user input (movement, shooting).
   - Receives real-time updates from the server for entity positions, scoring, level transitions, etc.

- **Data**:
   - Assets like textures, sprite sheets, audio files are kept in an `assets/` folder or a similar structure.
   - Game levels or wave definitions can be stored in code (C++ arrays, data-driven files, or via external config files).

---

## Adding a New Level

### Step 1: Update Level Spawning Logic

1. Open **`GameEngine.cpp`** (on the server side).
2. Look for methods such as:
   - `spawnEnemiesForLevel(int level)`
   - `switchToNextLevel()`
   - `updatePlayerScore()` (which may trigger a level transition based on a score threshold)

Example snippet from `GameEngine.cpp`:
```cpp
void GameEngine::spawnEnemiesForLevel(int level) {
    // You can specify how many enemies per level
    // Then push back to the enemySpawnQueue with relevant coordinates and delays
    const int ENEMIES_PER_LEVEL[] = {15, 15, 15};
    int nbEnemies = ENEMIES_PER_LEVEL[level - 1];

    for (int i = 0; i < nbEnemies; i++) {
        float delay = static_cast<float>(i) * 2.0f;
        auto x = static_cast<float>(820);
        auto y = static_cast<float>(rand() % 560);

        // Populate our queue with instructions on when/where to spawn each enemy
        enemySpawnQueue.push_back(PendingSpawn{delay, x, y, level, false});
    }
}
```
3. **Add your new level** to data structures:
   - Increase the array size (`ENEMIES_PER_LEVEL`) or create a new constant to define how many enemies or waves the new level will have.
   - If your level is beyond the existing index, expand arrays or logic accordingly.

### Step 2: Configure Enemy Waves

- If your new level has unique enemy patterns, you may want to update or create custom spawn logic.
- You can also modify the `spawnEnemy(float x, float y, int level, bool isBoss)` function to handle new enemy behavior (e.g., new movement speeds, bullet patterns, or unique visuals).

### Step 3: (Optional) Add Custom Assets for the Level

- If your level uses a distinct background or tile set, follow the steps in [Adding or Updating Textures](#adding-or-updating-textures) to integrate a new background image or sprite layer.
- You might also introduce new music specific to this level; see [Adding or Updating Audio](#adding-or-updating-audio).

---

## Adding a Boss Enemy

### Step 1: Extend Enemy Attributes

In `GameEngine.hpp`, there is a static map `enemyAttributes`:

```cpp
static inline const std::unordered_map<int, std::tuple<float, int, float>> enemyAttributes = {
    {1, {1, 5, 100.0f}},
    {2, {3, 5, 300.0f}},
    {3, {5, 5, 600.0f}}
};
```

- You can add a new entry here if your boss is a new type, or simply rely on the boolean flag `isBoss`.
- Some logic in `spawnEnemy(...)` or `setupEnemyRenderComponent(...)` might already handle `isBoss` differently. For example:
  ```cpp
  // If isBoss = true, we multiply HP, speed, or other parameters
  if (entities.getComponent<Enemy>(enemy).isBoss) {
      // unique boss logic
  }
  ```

### Step 2: Modify Spawning Logic

- Decide when or how a boss is spawned.
   - For instance, in `updatePlayerScore()`, once a certain threshold is met for a specific level, a boss can be spawned:
     ```cpp
     if (!hasBossIsDisplay && !bossOfLevelIsDead) {
         // Clear regular enemies
         enemySpawnQueue.clear();
         // Push back a boss spawn
         enemySpawnQueue.push_back(PendingSpawn{2.0f, 820.0f, 300.0f, currentLevel, true});
         hasBossIsDisplay = true;
     }
     ```
- If the boss is intended for a brand-new level, make sure `spawnEnemiesForLevel()` or your other wave logic calls that code.

### Step 3: Customize Boss Mechanics

- You can modify how the boss shoots by editing `ShootSystem.cpp` or adding new triggers in `handleEnemyShoot()`.
- Update the collision logic in `handleCollision(...)` if the boss has special damage rules.

---

## Adding or Updating Textures

### Step 1: Place Textures in the Assets Folder
- Copy your new `.png`, `.jpg`, or `.gif` files into an appropriate subfolder of `assets/`.  
  Example structure:
  ```
  assets/
  ├── sprites/
  │   ├── new_boss.png
  │   └── ...
  └── background/
      ├── level4_bg.png
  ```

### Step 2: Load the Texture in the Resource Manager
- Edit `loadResources()` (usually in **`Game.cpp`** or **`ResourceManager.cpp`**) to load the new texture:
  ```cpp
  // Example in loadResources()
  auto& resources = ResourceManager::getInstance();
  resources.loadTexture("boss_level4", "assets/sprites/new_boss.png");
  ```
- Use a unique string ID (e.g., `"boss_level4"`) that you can reference later.

### Step 3: Attach the Texture to an Entity
- On the **client side** (since rendering is handled there), when you receive an `ENTITY_UPDATE` for your boss entity, set the sprite:
  ```cpp
  // In handleEntityUpdate or setupEnemyRenderComponent
  if (type == <boss_type_id>) {
      renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("boss_level4"));
      // Configure frame width, animation data, etc.
  }
  ```

---

## Adding or Updating Audio

### Step 1: Place Audio Files in the Assets Folder
- Copy your `.wav`, `.ogg`, or `.mp3` files into `assets/audio/`, for example:
  ```
  assets/
  └── audio/
      ├── boss_music.mp3
      ├── level4_theme.ogg
      └── ...
  ```

### Step 2: Load Audio in Game or Resource Manager
- If your audio is background music, typically the **client** is responsible for playing it.
- For short sound effects, you might use `sf::SoundBuffer` and `sf::Sound`.
- For continuous background music, you might use `sf::Music`.

Example:
```cpp
if (!bossMusic.openFromFile("assets/audio/boss_music.mp3")) {
    std::cerr << "Error loading boss music\n";
} else {
    bossMusic.setLoop(true);
    bossMusic.setVolume(30.0f);
}
```

### Step 3: Play Audio in Code
- Call `bossMusic.play()` when the boss appears, or `musicGame.play()` in `initAudio()` for general background music.
- Adjust volume, looping, or other parameters as needed.

---

## Common Pitfalls and Tips

1. **Forgetting to Broadcast Updates**
   - When you add new entities (bosses, health packs, walls, etc.), the server must broadcast `ENTITY_UPDATE` so that clients can render them correctly.

2. **Overlapping IDs**
   - If you introduce new enemy types, ensure you do not reuse the same numeric type ID for different entities. Keep the IDs consistent between server and client.

3. **Animation Frame Width/Height**
   - Double-check `frameWidth`, `frameHeight`, and texture rectangles in your rendering code so sprites animate properly.

4. **Audio Format**
   - Some audio formats (`.mp3`, `.ogg`) might behave differently depending on the SFML version or platform. Use `.wav` or `.ogg` if you encounter issues with `.mp3`.

5. **Initialization Order**
   - Make sure your resources (textures, fonts, music) are loaded before attempting to use them. Typically, do this in an early initialization step like `initGame()` or `loadResources()`.

6. **Memory Constraints**
   - If you add many large textures or audio files, be mindful of memory usage.
   - Consider using streaming or lower-resolution assets.

---

## FAQ

1. **What if I want to store wave definitions in a file instead of in code?**
   - You can parse a JSON or XML file at server startup to populate `enemySpawnQueue`. This would allow you to manage waves outside of compiled code.

2. **How can I make the background scroll at different speeds for each level?**
   - Look at `BackgroundSystem`. You can configure `scrollSpeed` differently when creating background entities for each level.

3. **Where do I handle a boss’s unique behavior besides shooting and collisions?**
   - You can add logic in `GameEngine::update()`, or create a custom ECS system that checks for `isBoss` and applies special logic each frame.

---

## Further Reading

- **[SFML Documentation](https://www.sfml-dev.org/documentation.php)** – Official docs for graphics, audio, and input handling.
- **ECS Patterns** – [Medium article on ECS basics](https://medium.com/@adamgit/ecs-what-is-it-53b2f9b2c6f2).
- **C++ STL** – For `std::vector`, `std::unordered_map`, `std::unique_ptr`, etc.

If you have any questions or run into issues while adding new content, feel free to check our issues board on GitHub or ask in our project’s chat channel. Happy modding!
# R-Type Game Engine

**A game engine that roars!**

## Project Overview

This project is part of the Advanced C++ Knowledge Unit and focuses on creating a networked video game. The goal is to implement a **multi-threaded server** and a **graphical client** for a modern adaptation of the classic game **R-Type**, showcasing **game engine architecture**.

---

## Key Features

- **Networked Multiplayer:** Players collaborate or compete against each other online to battle the evil Bydos.
- **Game Engine Architecture:** Designed with clear modularity and decoupling between rendering, networking, and game logic subsystems.
- **Cross-Platform Support:** Runs on Linux and Windows with seamless cross-play between clients.

---

## Build and Run Instructions

### Requirements

- **Language:** C++
- **Build System:** CMake
- **Package Manager:** CMake CPM
- **Dependencies:**
    - SFML (for rendering/audio/input)

### Build Steps

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd <repository-directory>
   ```

2. Install dependencies:
   Voici les instructions spécifiques pour installer SFML sous Fedora et macOS :

   #### Fedora
    ```bash
    sudo dnf install sfml-devel
    ```

   #### macOS (via Homebrew)
    ```bash
    brew install sfml
    ```

3. Build the project:
   ```bash
   cmake -B build
   cmake --build build
   ```

4. Run the server and client:
   ```bash
   ./build/r-type_server
   ./build/r-type_client
   ```

---

## Gameplay

- **Objective:** Destroy the waves of Bydos with your spaceship and survive as long as possible.
- **Controls:** Use arrow keys for movement and spacebar for shooting.
- **Multiplayer:** Up to four players can join a game session via the network.
- **Enemies:** Includes monsters, obstacles, and powerful bosses like Dobkeratops.

---

## Development Roadmap

### Phase 1: Core Architecture & Prototype
- Implement the game engine's core systems: **Rendering**, **Networking**, and **Game Logic**.
- Deliver a playable R-Type prototype.

### Phase 2: Advanced Features
- **Engine Enhancements:** Improve modularity, add subsystems (e.g., physics, asset management).
- **Networking:** Optimize data transmission and implement client-server synchronization.
- **Gameplay Expansion:** Add new monsters, weapons, and levels with a focus on fun and replayability.

---

## Documentation

- **Protocol Documentation:** A detailed binary communication protocol for client-server interactions.
- **Developer Guide:** Architectural diagrams, tutorials, and coding conventions.
- **Contribution Guidelines:** Best practices for Git usage, CI/CD workflows, and code formatting.

---

## Accessibility

This project includes accessibility considerations for:
- **Visual Impairments:** Adjustable color schemes and screen-reader compatibility.
- **Motor Disabilities:** Configurable controls and simplified gameplay modes.

---

## License

This project is licensed under the **MIT License**. See the `LICENSE` file for details.

---

## Authors

- Arthur BLARINGHEM (server)
- Jean-Baptiste AZAN (server)
- Raphael SILLEGUE (graphical)
- Romain COLLIGNON (graphical)
- Théo LEMAIRE (server)

---

## Useful Links

- [R-Type Wikipedia](https://en.wikipedia.org/wiki/R-Type)
- [Original Gameplay Breakdown](https://shmups.wiki/library/R-Type)

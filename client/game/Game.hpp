/**
 * @file Game.hpp
 * @brief Defines the Game class for the R-Type project (client side).
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "shared/ecs/EntityManager.hpp"
#include "shared/systems/System.hpp"
#include "shared/systems/MouvementSystem.hpp"
#include "network/NetworkManager.hpp"
#include <systems/RenderSystem.hpp>
#include "shared/network/packetType.hpp"
#include "systems/AnimationSystem.hpp"
#include "gameComponents/backgroundComponent.hpp"
#include "systems/BackgroundSystem.hpp"
#include "menu/Menu.hpp"
#include "utils/GameState.hpp"
#include <functional>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <memory>
#include <iostream>
#include <thread>

namespace rtype {

    /**
     * @struct PlayerStats
     * @brief Holds statistics about the player.
     */
    struct PlayerStats {
        int best_time = 0;       ///< The best time achieved by the player in seconds.
        int total_games = 0;     ///< The total number of games played.
        int total_playtime = 0;  ///< The total accumulated playtime in seconds.
        float avg_score = 0;     ///< The average score the player has achieved.
        int current_level = 1;   ///< The current level the player has reached.
        int enemies_killed = 0;  ///< The total number of enemies killed by the player.
    };

    /**
     * @struct LeaderboardEntry
     * @brief Represents a single entry in the leaderboard.
     */
    struct LeaderboardEntry {
        std::string username;  ///< The username of the player.
        int score;             ///< The score achieved by the player.
        int level_reached;     ///< The highest level reached by the player.
        int time;              ///< The time taken by the player (or similar metric).
    };

    /**
     * @class Game
     * @brief Manages the core gameplay loop, network interactions, and rendering for the R-Type client.
     *
     * The Game class is responsible for:
     * - Initializing and running the main game loop (handle events, update, render).
     * - Managing network communication with the server.
     * - Keeping track of player statistics and leaderboard entries.
     * - Handling UI components such as menu and end-game screens.
     */
    class Game {
    public:
        /**
         * @brief Default constructor.
         *
         * Initializes the game window, loads fonts, and sets up packet handlers.
         * May throw std::runtime_error if assets fail to load.
         */
        Game();

        /**
         * @brief Runs the main game loop.
         *
         * This function contains the high-level state machine for:
         * - Displaying the menu.
         * - Connecting to the server.
         * - Transitioning to the main gameplay.
         * - Handling end-game states or disconnections.
         */
        void run();

    private:
        // =========================
        // == Player Stats & UI  ==
        // =========================

        PlayerStats playerStats; ///< Contains the current player's statistics.

        /**
         * @brief Updates the in-game text displays (score, life, time, etc.) based on the current player stats.
         */
        void updateStatsDisplay();

        sf::Text gameOverText;    ///< Text object displayed when the game is over (lose or victory).
        bool playerIsDead = false;///< Indicates if the local player has died.
        int playerLife = 3;       ///< Player's remaining lives.
        sf::Text lifeText;        ///< Displays the current life count on screen.

        bool endGame = false;     ///< Indicates if the game has ended (victory or forced stop).
        sf::Text endGameText;     ///< Text displayed at the end of the game (e.g., final stats).

        // =========================
        // == Audio Elements     ==
        // =========================

        sf::Music musicGame;   ///< Background music for the main game.
        sf::Music weaponSong;  ///< Sound effect for player shooting.

        // =========================
        // == Font & Text        ==
        // =========================

        sf::Font font;         ///< Global font used for UI text.
        sf::Text scoreText;    ///< Text displaying the player's score.
        int playerScore = 0;   ///< Numerical representation of the player's current score.
        sf::Text levelText;    ///< Text displaying the current level.
        int currentLevel = 1;  ///< Tracks the player's current level during the game.

        // =========================
        // == SFML Window & ECS  ==
        // =========================

        sf::RenderWindow window;                               ///< Main SFML window for rendering.
        EntityManager entities;                                 ///< ECS EntityManager to handle all entities.
        std::vector<std::unique_ptr<ISystem>> systems;         ///< List of all systems (e.g., rendering, movement).

        // =========================
        // == Networking         ==
        // =========================

        std::unique_ptr<network::NetworkClient> network; ///< Manages UDP communication with the server.

        /**
         * @brief Cleans up out-of-bound entities to keep the entity list tidy.
         *
         * Removes entities that are no longer relevant or that have moved far outside the visible game area.
         */
        void cleanupEntities();

        /**
         * @brief Processes leaderboard data received from the server.
         *
         * @param data   The raw packet data.
         * @param offset The offset in the data buffer at which the leaderboard data begins.
         */
        void handleLeaderboard(const std::vector<uint8_t> &data, size_t offset);

        /**
         * @brief Renders the leaderboard to the window.
         */
        void renderLeaderboard();

        // =========================
        // == Initialization     ==
        // =========================

        /**
         * @brief Initializes all game-related states, resources, and systems for actual gameplay.
         */
        void initGame();

        /**
         * @brief Creates the initial UI texts (score, life, game over messages, etc.).
         */
        void initGameTexts();

        /**
         * @brief Loads textures, fonts, and other resources.
         */
        void loadResources();

        /**
         * @brief Sets up the ECS systems (Movement, Rendering, etc.).
         */
        void setupSystems();

        /**
         * @brief Initializes in-game music and sound effects.
         */
        void initAudio();

        /**
         * @brief Stops network operations and resets the network client if needed.
         */
        void cleanupNetwork();

        /**
         * @brief Asks the player if they want to retry (not used in some flows).
         * @return true if the player wants to retry, false otherwise.
         */
        bool handleRetry();

        /**
         * @brief Connects to the server based on credentials/IP info from the Menu.
         */
        void connectToServer();

        /**
         * @brief Creates background entities (parallax layers, etc.).
         */
        void createBackgroundEntities();

        /**
         * @brief Processes an incoming message from the server.
         *
         * @param data   The received packet data.
         * @param sender The endpoint from which the packet was sent.
         */
        void handleNetworkMessage(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &sender);

        /**
         * @brief Sets up the parallax background for the menu screen.
         */
        void initMenuBackground();

        // =========================
        // == Game Loop Methods  ==
        // =========================

        /**
         * @brief Handles player and window events (keyboard, closing window, etc.).
         */
        void handleEvents();

        /**
         * @brief Performs frame logic updates (movement systems, collision checks, etc.).
         */
        void update();

        /**
         * @brief Renders the current scene to the window.
         */
        void render();

        /**
         * @brief Displays the main menu until the user chooses to play or close the game.
         */
        void displayMenu();

        /**
         * @brief Displays final stats when the game ends (e.g., victory or defeat).
         */
        void displayFinalStats();

        // =========================
        // == Packet Handlers    ==
        // =========================

        using PacketHandler = std::function<void(const std::vector<uint8_t>&, size_t offset)>;
        std::unordered_map<network::PacketType, PacketHandler> packetHandlers; ///< Maps packet types to their handling functions.

        /**
         * @brief Initializes the mapping between PacketType and the corresponding handler functions.
         */
        void initPacketHandlers();

        /**
         * @brief Updates an existing entity's components based on server data.
         *
         * @param entity       The entity to update.
         * @param entityUpdate The update data from the server.
         */
        void updateExistingEntity(EntityID entity, const network::EntityUpdatePacket *entityUpdate);

        /**
         * @brief Handles a server response for a connection request.
         *
         * @param data   The received packet data.
         * @param offset The offset at which the data for the connect response begins.
         */
        void handleConnectResponse(const std::vector<uint8_t>& data, size_t offset);

        /**
         * @brief Handles an entity death event from the server.
         *
         * @param data   The received packet data.
         * @param offset The offset at which the data for the entity death begins.
         */
        void handleEntityDeath(const std::vector<uint8_t>& data, size_t offset);

        /**
         * @brief Handles an entity update event from the server (position, velocity, etc.).
         *
         * @param data   The received packet data.
         * @param offset The offset at which the data for the entity update begins.
         */
        void handleEntityUpdate(const std::vector<uint8_t>& data, size_t offset);

        /**
         * @brief Handles a BestScorePacket from the server, updating the player's best scores/time.
         *
         * @param data   The received packet data.
         * @param offset The offset at which the best score data begins.
         */
        void handleBestScore(const std::vector<uint8_t>& data, size_t offset);

        /**
         * @brief Handles a GameStatsPacket from the server, updating the player's game stats.
         *
         * @param data   The received packet data.
         * @param offset The offset at which the game stats data begins.
         */
        void handleGameStats(const std::vector<uint8_t>& data, size_t offset);

        /**
         * @brief Handles an end-game notification (Victory).
         *
         * @param data   The received packet data.
         * @param offset The offset at which the end-game data begins.
         */
        void handleEndGame(const std::vector<uint8_t>& data, size_t offset);

        /**
         * @brief Handles a lose-game notification.
         *
         * @param data   The received packet data.
         * @param offset The offset at which the lose-game data begins.
         */
        void handleLose(const std::vector<uint8_t>& data, size_t offset);

        // =========================
        // == Enemy / Wall Setup ==
        // =========================

        /**
         * @brief Sets up rendering and animation for various enemy types.
         *
         * @param entity The enemy entity ID.
         * @param type   An integer representing the enemy type.
         * @param renderComp A reference to the entity's RenderComponent to configure.
         */
        void setupEnemyRenderComponent(EntityID entity, int type, RenderComponent &renderComp);

        /**
         * @brief Configures sprite sheets and frame data for specific enemy animations.
         *
         * @param type       Enemy type ID.
         * @param renderComp The RenderComponent to modify.
         */
        void setupEnemyAnimation(int type, RenderComponent &renderComp);

        /**
         * @brief Sets up rendering for a wall entity.
         *
         * @param entity     The wall entity ID.
         * @param renderComp A reference to the RenderComponent to configure.
         */
        void setupWallRenderComponent(EntityID entity, RenderComponent &renderComp);

        // =========================
        // == Menu & State       ==
        // =========================

        Menu menu;        ///< The main menu system for the game (start, settings, etc.).
        sf::Event event{};///< SFML event used throughout the game loop.

        EntityID myPlayerId = 0;          ///< The client entity ID assigned by the server.
        GameState currentState = GameState::MENU; ///< Tracks the current game state (menu, playing, etc.).

        // =========================
        // == Additional UI Text ==
        // =========================

        sf::Text statsText;       ///< Displays extended stats such as enemies killed.
        sf::Text bestScoreText;   ///< Displays the best score/time.
        sf::Text timeText;        ///< Displays the current playtime.

        std::chrono::steady_clock::time_point gameStartTime; ///< Time point when the game started.

        // =========================
        // == Leaderboard        ==
        // =========================

        std::vector<LeaderboardEntry> leaderboardEntries; ///< A container for the leaderboard data from the server.
        Button backToMenuButton;                          ///< A button to go back to the menu from the leaderboard.

        // =========================
        // == Timing             ==
        // =========================

        std::chrono::steady_clock::time_point lastUpdate =
            std::chrono::steady_clock::now(); ///< Used to measure delta time between frames.
    };

} // namespace rtype

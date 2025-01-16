/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** GameEngine
*/
#pragma once

#include "../shared/ecs/EntityManager.hpp"
#include "../shared/systems/System.hpp"
#include "../shared/abstracts/AEngine.hpp"
#include "../shared/systems/ShootSystem.hpp"
#include "../shared/network/packetType.hpp"
#include "../network/NetworkManager.hpp"
#include "../shared/systems/MouvementSystem.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <map>

namespace rtype::game {

    /**
     * @class GameEngine
     * @brief Manages the game logic and state.
     */
    class GameEngine : public engine::AEngine {
    public:
        /**
         * @brief Constructs a new GameEngine object.
         * @param networkManager Reference to the NetworkManager.
         */
        GameEngine(network::NetworkManager& networkManager);

        /**
         * @brief Broadcasts the current world state to all clients.
         */
        void broadcastWorldState();

        /**
         * @brief Creates a new player entity.
         * @param sender The endpoint of the player.
         * @return The ID of the created player entity.
         */
        EntityID createNewPlayer(const asio::ip::udp::endpoint& sender);

        /**
         * @brief Updates the game state.
         */
        void update() override;

        /**
         * @brief Handles incoming messages.
         * @param data The data of the message.
         * @param sender The endpoint of the sender.
         */
        void handleMessage(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender) override;
    private:
        ShootSystem shoot_system_; ///< System for handling shooting mechanics.
        std::vector<std::unique_ptr<ISystem>> systems; ///< List of systems in the game.
        EntityManager entities; ///< Manages all entities in the game.
        network::NetworkManager& network; ///< Reference to the network manager.
        std::unordered_map<std::string, EntityID> playerEntities; ///< Maps player IDs to entity IDs.
        std::chrono::steady_clock::time_point lastUpdate; ///< Time point of the last update.
        std::chrono::steady_clock::time_point lastUpdateEnemiesShoot; ///< Time point of the last enemy shoot update.
        std::chrono::steady_clock::time_point lastUpdateWallShoot; ///< Time point of the last wall shoot update.
        std::chrono::steady_clock::time_point lastUpdateHealthPack; ///< Time point of the last health pack update.
        float speed = 200.0f; ///< Speed of the entities.
        std::vector<PendingSpawn> enemySpawnQueue; ///< Queue of enemies to be spawned.
        std::random_device rd; ///< Random device for generating random numbers.
        std::mt19937 gen; ///< Mersenne Twister random number generator.
        std::uniform_real_distribution<float> dis; ///< Uniform real distribution for random numbers.
        static inline const std::unordered_map<int, std::tuple<float, int, float>> enemyAttributes = {
            {1, {1, 5, 100.0f}},
            {2, {3, 5, 300.0f}},
            {3, {5, 5, 600.0f}}
        }; ///< Attributes of enemies based on their level.
        int currentLevel = 1; ///< Current level of the game.
        void initializeLevel();
        /**
         * @brief Switches to the next level.
         */
        void switchToNextLevel();

        /**
         * @brief Spawns enemies for the given level.
         * @param level The level for which to spawn enemies.
         */
        void spawnEnemiesForLevel(int level);

        /**
         * @brief Handles the spawning of health packs.
         */
        void handleHealthPackSpawns();

        const std::map<int, int> SCORE_THRESHOLDS = {
            {1, 10},
            {2, 20},
            {3, 30}
        }; ///< Score thresholds for each level.

        /**
         * @brief Handles incoming network messages.
         * @param data The data of the message.
         * @param sender The sender's address.
         * @param clientId The ID of the client.
         */
        void handleNetworkMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender, const std::string& clientId);

        /**
         * @brief Handles player disconnection.
         * @param clientId The ID of the client.
         */
        void handlePlayerDisconnection(const std::string& clientId);

        /**
         * @brief Handles the spawning of enemies.
         * @param dt The delta time since the last update.
         */
        void handleEnemySpawns(float dt);

        /**
         * @brief Handles the spawning of walls.
         */
        void handleWallSpawns();

        /**
         * @brief Handles enemy shooting.
         */
        void handleEnemyShoot();

        /**
         * @brief Handles collisions between entities.
         */
        void handleCollisions();

        /**
         * @brief Handles collision between a missile and an enemy.
         * @param missile The ID of the missile entity.
         * @param enemy The ID of the enemy entity.
         */
        void handleCollision(EntityID missile, EntityID enemy);

        /**
         * @brief Handles collision between a missile and a player.
         * @param missile The ID of the missile entity.
         * @param player The ID of the player entity.
         */
        void handleCollisionPlayer(EntityID missile, EntityID player);

        /**
         * @brief Updates the player's score.
         */
        void updatePlayerScore();

        /**
         * @brief Broadcasts the end game state to all clients.
         */
        void broadcastEndGameState();

        /**
         * @brief Checks for collision between two circular entities.
         * @param pos1 The position of the first entity.
         * @param radius1 The radius of the first entity.
         * @param pos2 The position of the second entity.
         * @param radius2 The radius of the second entity.
         * @return True if the entities collide, false otherwise.
         */
        bool checkCollision(const Position& pos1, float radius1, const Position& pos2, float radius2);

        /**
         * @brief Checks for collision between a circular entity and a rectangular entity.
         * @param circlePos The position of the circular entity.
         * @param radius The radius of the circular entity.
         * @param rectPos The position of the rectangular entity.
         * @param rectWidth The width of the rectangular entity.
         * @param rectHeight The height of the rectangular entity.
         * @return True if the entities collide, false otherwise.
         */
        bool checkCollisionRect(const Position& circlePos, float radius, const Position& rectPos, float rectWidth, float rectHeight);

        /**
         * @brief Creates a packet for entity death.
         * @param missile The ID of the missile entity.
         * @param enemy The ID of the enemy entity.
         * @return The created packet.
         */
        [[nodiscard]] std::vector<uint8_t> createEntityDeathPacket(EntityID missile, EntityID enemy) const;

        /**
         * @brief Creates a packet for the end game state.
         * @return The created packet.
         */
        [[nodiscard]] std::vector<uint8_t> createEndGamePacket() const;

        /**
         * @brief Gets the attributes of an enemy based on its level.
         * @param level The level of the enemy.
         * @return A tuple containing the attributes of the enemy.
         */
        static std::tuple<float, int, float> getEnemyAttributes(int level);

        /**
         * @brief Spawns an enemy at the given position and level.
         * @param x The x-coordinate of the enemy.
         * @param y The y-coordinate of the enemy.
         * @param level The level of the enemy.
         */
        void spawnEnemy(float x, float y, int level);

        /**
         * @brief Spawns a wall at the given position.
         * @param x The x-coordinate of the wall.
         * @param y The y-coordinate of the wall.
         */
        void spawnWall(float x, float y);
    };

} // namespace rtype::game
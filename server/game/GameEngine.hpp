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
    class GameEngine : public engine::AEngine {
        public:
            GameEngine(network::NetworkManager& networkManager);

            void broadcastWorldState();
            EntityID createNewPlayer(const asio::ip::udp::endpoint& sender);
            void update() override;
            void handleMessage(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender) override;

        private:
            // Systèmes et gestion des entités
            ShootSystem shoot_system_;
            std::vector<std::unique_ptr<ISystem>> systems;
            EntityManager entities;

            // Gestion du réseau
            network::NetworkManager& network;
            std::unordered_map<std::string, EntityID> playerEntities;

            // Gestion du temps et de la vitesse
            std::chrono::steady_clock::time_point lastUpdate;
            std::chrono::steady_clock::time_point lastUpdateEnemiesShoot;
            std::chrono::steady_clock::time_point lastUpdateWallShoot;
            float speed = 200.0f;

            // File d'attente pour les spawns d'ennemis
            std::vector<PendingSpawn> enemySpawnQueue;

            // Initialise le générateur aléatoire
            std::random_device rd;
            std::mt19937 gen;
            std::uniform_real_distribution<float> dis;

            // Niveau possible des enemies -> {lvl, {life, damage, speed shoot}}
            static inline const std::unordered_map<int, std::tuple<float, int, float>> enemyAttributes = {
                {1, {1, 5, 100.0f}},
                {2, {3, 5, 300.0f}},
                {3, {5, 5, 600.0f}}
            };

            // gestion des level
            int currentLevel = 1;
            void switchToNextLevel();
            void spawnEnemiesForLevel(int level);
            void spawnHealthPack();

            const std::map<int, int> SCORE_THRESHOLDS = {
                {1, 10},  // Score 10 pour passer au niveau 2
                {2, 20},  // Score 20 pour passer au niveau 3
                {3, 30}   // Score final
            };


            // Méthodes de gestion des messages réseau
            void handleNetworkMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender, const std::string& clientId);
            void handlePlayerDisconnection(const std::string& clientId);

            // Méthodes de mise à jour du jeu
            void handleEnemySpawns(float dt);
            void handleWallSpawns();
            void handleEnemyShoot();
            void handleCollisions();
            void handleCollision(EntityID missile, EntityID enemy);
            void handleCollisionPlayer(EntityID missile, EntityID player);
            void updatePlayerScore();
            void broadcastEndGameState();

            // Méthodes utilitaires
            bool checkCollision(const Position& pos1, float radius1, const Position& pos2, float radius2);
            bool checkCollisionRect(const Position& circlePos, float radius, const Position& rectPos, float rectWidth, float rectHeight);
            std::vector<uint8_t> createEndGamePacket() const;
            static std::tuple<float, int, float> getEnemyAttributes(int level);
            void spawnEnemy(float x, float y, int level);
            void spawnWall(float x, float y, int level);
            std::string formatClientId(const sockaddr_in& client) const;

            // Méthodes de gestion des paquets
            std::vector<uint8_t> createPacket(network::PacketType type, const auto& data) const;
            std::vector<uint8_t> createEntityDeathPacket(EntityID missile, EntityID enemy) const;
        };

} // namespace rtype::game
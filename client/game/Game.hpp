/**
* \file Game.hpp
* \brief Defines the Game class for the R-type project.
**/

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

namespace rtype {
    struct PlayerStats {
        int best_time = 0;
        int total_games = 0;
        int total_playtime = 0;
        float avg_score = 0;
        int current_level = 1;
        int enemies_killed = 0;
    };

    struct LeaderboardEntry {
        std::string username;
        int score;
        int level_reached;
        int time;
    };
    /**
     * \class Game
     * \brief Class representing the game itself in the client side of the projet.
     */
    class Game {
    public:
        Game();

        void run();

    private:
        PlayerStats playerStats;
        void updateStatsDisplay();
        sf::Text gameOverText;
        bool playerIsDead = false;
        int playerLife = 3;
        sf::Text lifeText;
        bool endGame = false;
        sf::Text endGameText;
        sf::Music musicGame;
        sf::Music weaponSong;
        sf::Font font;
        sf::Text scoreText;
        int playerScore = 0;
        sf::Text levelText;
        int currentLevel = 1;
        sf::RenderWindow window;
        EntityManager entities;
        std::vector<std::unique_ptr<ISystem> > systems;
        std::unique_ptr<network::NetworkClient> network;
        std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
        void cleanupEntities();

        void handleLeaderboard(const std::vector<uint8_t> &data, size_t offset);

        void renderLeaderboard();

        void initGame();
        void initGameTexts();
        void loadResources();
        void setupSystems();
        void initAudio();
        void cleanupNetwork();
        bool handleRetry();
        void connectToServer();
        void createBackgroundEntities();
        void handleNetworkMessage(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &sender);
        void initMenuBackground();
        void handleEvents();
        void update();
        void render();
        void displayMenu();
        void displayFinalStats();

        using PacketHandler = std::function<void(const std::vector<uint8_t>&, size_t offset)>;
        std::unordered_map<network::PacketType, PacketHandler> packetHandlers;
        void initPacketHandlers();

        void updateExistingEntity(EntityID entity, const network::EntityUpdatePacket *entityUpdate);

        void handleConnectResponse(const std::vector<uint8_t>& data, size_t offset);
        void handleEntityDeath(const std::vector<uint8_t>& data, size_t offset);
        void handleEntityUpdate(const std::vector<uint8_t>& data, size_t offset);
        void handleBestScore(const std::vector<uint8_t>& data, size_t offset);
        void handleGameStats(const std::vector<uint8_t>& data, size_t offset);
        void handleEndGame(const std::vector<uint8_t>& data, size_t offset);

        void setupEnemyRenderComponent(EntityID entity, int type, RenderComponent &renderComp);

        void setupEnemyAnimation(int type, RenderComponent &renderComp);

        void setupWallRenderComponent(EntityID entity, RenderComponent &renderComp);

        Menu menu;
        sf::Event event{};
        EntityID myPlayerId = 0;
        GameState currentState = GameState::MENU;
        sf::Text statsText;
        sf::Text bestScoreText;
        sf::Text timeText;
        std::chrono::steady_clock::time_point gameStartTime;
        std::vector<LeaderboardEntry> leaderboardEntries;
        Button backToMenuButton;
    };
}

/**
* \file Game.hpp
* \brief Defines the Game class for the R-type project.
**/

#pragma once
#include <SFML/Graphics.hpp>
#include "shared/ecs/EntityManager.hpp"
#include "shared/systems/System.hpp"
#include "shared/systems/MouvementSystem.hpp"
#include "network/NetworkManager.hpp"
#include <systems/RenderSystem.hpp>
#include "network/packetType.hpp"
#include "systems/AnimationSystem.hpp"
#include "gameComponents/backgroundComponent.hpp"
#include "systems/BackgroundSystem.hpp"
#include "menu/Menu.hpp"

namespace rtype {
    /**
     * \class Game
     * \brief Class representing the game itself in the client side of the projet.
     */
    class Game {
    public:
        Game();
        void run();
    private:
        int playerLife = 3;
        sf::Text lifeText;
        bool endGame = false;
        sf::Text endGameText;
        sf::Font font;
        sf::Text scoreText;
        int playerScore = 0;
        sf::Text levelText;
        int currentLevel = 1;
        sf::RenderWindow window;
        EntityManager entities;
        std::vector<std::unique_ptr<ISystem>> systems;
        network::NetworkClient network;
        std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
void handleNetworkMessage(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender);        void handleEvents();
        void update();
        void render();
        void displayMenu();
        Menu menu;
        sf::Event event;
        EntityID myPlayerId = 0;
    };
}
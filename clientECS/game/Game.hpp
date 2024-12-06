#pragma once
#include <SFML/Graphics.hpp>
#include "shared/ecs/EntityManager.hpp"
#include "shared/systems/System.hpp"
#include "network/NetworkManager.hpp"
#include "systems/RenderSystem.hpp"

namespace rtype {

class Game {
    public:
        Game();
        void run();

    private:
        sf::RenderWindow window;
        EntityManager entities;
        std::vector<std::unique_ptr<ISystem>> systems;
        network::NetworkClient network;
        std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
        void handleNetworkMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender);
        void handleEvents();
        void update();
        void render();
    };

} // namespace rtype
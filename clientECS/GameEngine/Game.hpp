#pragma once
#include <SFML/Graphics.hpp>
#include "shared/ecs/EntityManager.hpp"
#include "shared/systems/System.hpp"
#include "systems/NetworkSystem.hpp"
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
        NetworkClient network;
        std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();

        void handleEvents();
        void update();
        void render();
    };

} // namespace rtype
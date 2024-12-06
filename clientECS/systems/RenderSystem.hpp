#pragma once
#include <SFML/Graphics.hpp>
#include "shared/ecs/EntityManager.hpp"
#include "shared/systems/System.hpp"
#include "network/NetworkManager.hpp"

namespace rtype {
   class RenderSystem : public ISystem {
    public:
        explicit RenderSystem(sf::RenderWindow& window) : window(window) {}
        void update(EntityManager& manager, float dt) override;
    private:
        sf::RenderWindow& window;
    };
}
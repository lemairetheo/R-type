#pragma once
#include <SFML/Graphics.hpp>
#include "shared/ecs/EntityManager.hpp"
#include "shared/systems/System.hpp"
#include "manager/ResourceManager.hpp"
#include "gameComponents/RenderComponent.hpp"

namespace rtype {
   class RenderSystem : public ISystem {
    public:
       explicit RenderSystem(sf::RenderWindow& window) : window(window) {
           auto& resources = ResourceManager::getInstance();
           resources.loadTexture("player", "assets/sprites/ship.gif");
       }

       void update(EntityManager& manager, float dt) override;
   private:
        sf::RenderWindow& window;
    };
}
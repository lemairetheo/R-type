//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#include "RenderSystem.hpp"

namespace rtype {
    void RenderSystem::update(EntityManager& manager, float dt) {
        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (manager.hasComponent<Position>(entity)) {
                const auto& pos = manager.getComponent<Position>(entity);
                sf::RectangleShape shape(sf::Vector2f(20, 20));
                shape.setPosition(pos.x, pos.y);
                shape.setFillColor(sf::Color::Red);
                window.draw(shape);
            }
        }
    }
}
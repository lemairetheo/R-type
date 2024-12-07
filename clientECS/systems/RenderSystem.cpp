//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

// RenderSystem.cpp
#include "RenderSystem.hpp"
#include "GameComponents/RenderComponent.hpp"

namespace rtype {
    void RenderSystem::update(EntityManager& manager, float dt) {
        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (manager.hasComponent<Position>(entity) &&
                manager.hasComponent<RenderComponent>(entity)) {
                const auto& pos = manager.getComponent<Position>(entity);
                auto& render = manager.getComponent<RenderComponent>(entity);

                // Update sprite position
                render.sprite.setPosition(pos.x, pos.y);
                window.draw(render.sprite);
                }
        }
    }
}
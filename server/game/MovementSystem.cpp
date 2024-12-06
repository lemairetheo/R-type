//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#include "MovementSystem.hpp"

namespace rtype::game {
    void update(rtype::EntityManager& manager, float dt) {
        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (manager.hasComponent<Position>(entity) && manager.hasComponent<Velocity>(entity)) {
                auto& pos = manager.getComponent<Position>(entity);
                const auto& vel = manager.getComponent<Velocity>(entity);

                pos.x += vel.dx * dt;
                pos.y += vel.dy * dt;
            }
        }
    }
}
#pragma once
#include "../../shared/ecs/EntityManager.hpp"
#include "../../shared/systems/System.hpp"

namespace rtype::game {
    class MovementSystem : public ISystem {
    public:
        void update(EntityManager& manager, float dt) override {
            for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
                if (manager.hasComponent<Position>(entity) && manager.hasComponent<Velocity>(entity)) {
                    auto& pos = manager.getComponent<Position>(entity);
                    const auto& vel = manager.getComponent<Velocity>(entity);
                    pos.x += vel.dx * dt;
                    pos.y += vel.dy * dt;
                }
            }
        }
    };
}
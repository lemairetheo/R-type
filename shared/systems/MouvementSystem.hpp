#include "System.hpp"
#include <iostream>

namespace rtype {
    class MovementSystem : public ISystem {
    public:
        void update(EntityManager& manager, float dt) override {
            for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
                if (manager.hasComponent<Position>(entity) && manager.hasComponent<Velocity>(entity)) {
                    auto& pos = manager.getComponent<Position>(entity);
                    auto& vel = manager.getComponent<Velocity>(entity);
                    auto& proj = manager.getComponent<Projectile>(entity);

                    pos.x += vel.dx * dt;
                    pos.y += vel.dy * dt;

                    if (manager.hasComponent<Projectile>(entity)) {
                        if (pos.x > 795)
                            pos.x = 900;
                        if (pos.y > 795)
                            proj.isActive = false;
                    } else {
                        pos.x = std::clamp(pos.x, 0.0f, 795.0f);
                        pos.y = std::clamp(pos.y, 0.0f, 590.0f);
                    }
                }
            }
        }
    };
}
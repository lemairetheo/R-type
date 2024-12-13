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
                        std::cout << pos.x << " " << pos.y << " entity : " << entity << std::endl;
                        if (pos.x > 795) pos.x = 900;
                        if (pos.y > 795) proj.isActive = false;
                    } else {
                        if (pos.x < 0.0f) pos.x = 0.0f;
                        if (pos.x > 795) pos.x = 795;
                        if (pos.y < 0.0f) pos.y = 0.0f;
                        if (pos.y > 590) pos.y = 590;
                    }
                }
            }
        }
    };
}
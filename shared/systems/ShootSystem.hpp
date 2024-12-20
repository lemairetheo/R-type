#include "System.hpp"
#include "../ecs/EntityManager.hpp"
#include "../ecs/Component.hpp"
#include <memory>
#include <chrono>

namespace rtype {

    class ShootSystem {
    public:
        ShootSystem() : lastShootTime(std::chrono::steady_clock::now()) {}

        void update(EntityManager& entities, EntityID entity) {
            handleShoot(entity, entities);
        }

        void handleShoot(EntityID entity, EntityManager& entities) {
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<float> elapsed = now - lastShootTime;

            if (elapsed.count() >= 0.2f) {
                lastShootTime = now;

                if (entities.hasComponent<Position>(entity)) {
                    const auto& position = entities.getComponent<Position>(entity);

                    EntityID projectile = entities.createEntity();

                    entities.addComponent(projectile, Position{position.x, position.y});
                    entities.addComponent(projectile, Velocity{300.0f, 0.0f});
                    entities.addComponent(projectile, Projectile{10.0f, true});
                }
            }
        }

    private:
        std::chrono::steady_clock::time_point lastShootTime;
    };

}

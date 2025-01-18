#include "System.hpp"
#include "../ecs/EntityManager.hpp"
#include "../ecs/Component.hpp"
#include <memory>
#include <chrono>

namespace rtype {

    class ShootSystem {
    public:
        ShootSystem() :
            lastShootTime(std::chrono::steady_clock::now()),
            lastUltimateTime(std::chrono::steady_clock::now()) {}

        void update(EntityManager& entities, EntityID entity, bool ultimate, float shootY) {
            handleShoot(entity, entities, ultimate, shootY);
        }

        void handleShoot(EntityID entity, EntityManager& entities, bool ultimate, float shootY) {
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<float> elapsedShoot = now - lastShootTime;
            std::chrono::duration<float> elapsedUltimate = now - lastUltimateTime;

            bool canShoot = elapsedShoot.count() >= 0.2f;
            bool canUltimate = elapsedUltimate.count() >= 5.0f;

            if (canShoot && (!ultimate || (ultimate && canUltimate))) {
                if (entities.hasComponent<Player>(entity)) {
                    lastShootTime = now;
                    if (ultimate) {
                        lastUltimateTime = now;
                    }
                }

                if (entities.hasComponent<Position>(entity)) {
                    const auto& position = entities.getComponent<Position>(entity);

                    EntityID projectile = entities.createEntity();

                    entities.addComponent(projectile, Position{position.x, position.y});
                    if (entities.hasComponent<Player>(entity)) {
                        if (ultimate) {
                            entities.addComponent(projectile, Velocity{350.0f, shootY});
                            entities.addComponent(projectile, Projectile{5.0f, 0, true, true});
                        } else {
                            entities.addComponent(projectile, Velocity{300.0f, shootY});
                            entities.addComponent(projectile, Projectile{1.0f, 0, true, false});
                        }
                    } else if (entities.hasComponent<Enemy>(entity)) {
                        entities.addComponent(projectile, Velocity{ entities.getComponent<Enemy>(entity).speedShoot * -1, shootY});
                        if (ultimate) {
                            entities.addComponent(projectile, Projectile{5.0f, 2, true, true});
                        } else {
                            entities.addComponent(projectile, Projectile{1.0f, 2, true, false});
                        }
                    }
                }
            }
        }

    private:
        std::chrono::steady_clock::time_point lastShootTime;
        std::chrono::steady_clock::time_point lastUltimateTime;
    };
}
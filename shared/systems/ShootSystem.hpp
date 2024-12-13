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
            // Vérifier si suffisamment de temps s'est écoulé depuis le dernier tir
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<float> elapsed = now - lastShootTime;

            if (elapsed.count() >= 0.2f) {  // Si 1 seconde s'est écoulée
                lastShootTime = now;  // Réinitialiser le temps

                if (entities.hasComponent<Position>(entity)) {
                    const auto& position = entities.getComponent<Position>(entity);

                    EntityID projectile = entities.createEntity();
                    entities.addComponent(projectile, Position{position.x, position.y});
                    entities.addComponent(projectile, Velocity{300.0f, 0.0f});
                    entities.addComponent(projectile, Projectile{10.0f, true});

                    std::cout << "Entity " << entity << " fired a projectile!" << std::endl;
                }
            }
        }

    private:
        std::chrono::steady_clock::time_point lastShootTime;  // Variable non statique pour suivre le dernier tir
    };

}

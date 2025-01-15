#include "System.hpp"
#include <iostream>

/**
 * @class MovementSystem
 * @brief A system that manages the movement and collision of entities in the game world.
*/
namespace rtype {
    class MovementSystem : public ISystem {
    public:
        /**
         * @brief Updates the position of entities based on their velocity and handles collisions (+ auto move of enemies with walls).
         *
         * When an enemy encounters a wall, it increases its velocity in the Y direction to move down and avoid collision.
         *
         * @param manager The EntityManager that provides access to entities and their components.
         * @param dt The delta time since the last update.
        */
        void update(EntityManager& manager, float dt) override {
            auto walls = manager.getEntitiesWithComponents<Wall>();

            for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
                if (manager.hasComponent<Position>(entity) && manager.hasComponent<Velocity>(entity)) {
                    auto& pos = manager.getComponent<Position>(entity);
                    auto& vel = manager.getComponent<Velocity>(entity);
                    auto& proj = manager.getComponent<Projectile>(entity);

                    if (manager.hasComponent<Projectile>(entity)) {
                        pos.x += vel.dx * dt;
                        pos.y += vel.dy * dt;
                    } else if (!manager.hasComponent<Wall>(entity)) {
                        bool hasCollided = false;
                        for (EntityID wall : walls) {
                            const auto& wallPos = manager.getComponent<Position>(wall);
                            // Check collision for enemies
                            if (manager.hasComponent<Enemy>(entity) && checkCollisionRect({pos.x + (vel.dx * dt), pos.y + (vel.dy * dt)}, 25.0f, wallPos, 80.0f, 80.0f)) { // Auto move enemies
                                pos.y += 5 * dt;
                            }
                            // General collision detection (enemies and players)
                            if (checkCollisionRect({pos.x + (vel.dx * dt), pos.y + (vel.dy * dt)}, 13.0f, wallPos, 20.0f, 60.0f)) {
                                hasCollided = true;
                                break;
                            }
                        }
                        if (!hasCollided) {
                            pos.x += vel.dx * dt;
                            pos.y += vel.dy * dt;
                        }
                    }

                    // Not leave the map for entities
                    if (manager.hasComponent<Projectile>(entity)) {
                        if (pos.x > 795)
                            pos.x = 900;
                        if (pos.y > 795)
                            proj.isActive = false;
                    } else {
                        if (manager.hasComponent<Enemy>(entity))
                            pos.x = std::clamp(pos.x, -20.0f, 795.0f); // Effect leave map but not real leave
                        else
                            pos.x = std::clamp(pos.x, 0.0f, 795.0f);
                        pos.y = std::clamp(pos.y, 0.0f, 590.0f);
                        if (manager.hasComponent<Enemy>(entity) && manager.getComponent<Position>(entity).x < -15) // Destroy enemies has finish the map
                            manager.destroyEntity(entity);
                    }
                }
            }
        }

        /**
         * @brief Checks if a circle and a rectangle are colliding.
         *
         * @param circlePos The position of the circle's center.
         * @param radius The radius of the circle.
         * @param rectPos The position of the top-left corner of the rectangle.
         * @param rectWidth The width of the rectangle.
         * @param rectHeight The height of the rectangle.
         * @return True if the circle and rectangle are colliding, false otherwise.
         */
        bool checkCollisionRect(const Position& circlePos, float radius, const Position& rectPos, float rectWidth, float rectHeight) {
            float closestX = std::max(rectPos.x, std::min(circlePos.x, rectPos.x + rectWidth));
            float closestY = std::max(rectPos.y, std::min(circlePos.y, rectPos.y + rectHeight));

            float dx = circlePos.x - closestX;
            float dy = circlePos.y - closestY;

            return (dx * dx + dy * dy) <= (radius * radius);
        }

    };
}  // namespace rtype

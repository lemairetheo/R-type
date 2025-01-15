#include "System.hpp"
#include <iostream>

namespace rtype {
    class MovementSystem : public ISystem {
    public:
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
                            if (manager.hasComponent<Enemy>(entity) && checkCollisionRect({pos.x + (vel.dx * dt), pos.y + (vel.dy * dt)}, 25.0f, wallPos, 80.0f, 80.0f)) {
                                pos.y += 5 * dt;
                            }

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

        bool checkCollisionRect(const Position& circlePos, float radius, const Position& rectPos, float rectWidth, float rectHeight) {
            float closestX = std::max(rectPos.x, std::min(circlePos.x, rectPos.x + rectWidth));
            float closestY = std::max(rectPos.y, std::min(circlePos.y, rectPos.y + rectHeight));

            float dx = circlePos.x - closestX;
            float dy = circlePos.y - closestY;

            return (dx * dx + dy * dy) <= (radius * radius);
        }


    };
}
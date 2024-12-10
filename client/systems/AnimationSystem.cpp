//
// Created by Jean-Baptiste  Azan on 07/12/2024.
//

#include "AnimationSystem.hpp"

namespace rtype {
    void AnimationSystem::update(EntityManager& manager, float dt) {
        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (manager.hasComponent<RenderComponent>(entity)) {
                auto& render = manager.getComponent<RenderComponent>(entity);

                render.animationTimer += dt;
                if (render.animationTimer >= render.frameTime) {
                    render.animationTimer = 0;
                    render.currentFrame = (render.currentFrame + 1) % render.frameCount;

                    int xPos = render.currentFrame * render.frameWidth;
                    render.sprite.setTextureRect(sf::IntRect(xPos, 0, render.frameWidth, render.frameHeight));
                }
            }
        }
    }
}
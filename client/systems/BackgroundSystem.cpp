//
// Created by Jean-Baptiste  Azan on 07/12/2024.
//

#include "BackgroundSystem.hpp"


namespace rtype {
    void BackgroundSystem::update(EntityManager& manager, float dt) {
        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (manager.hasComponent<BackgroundComponent>(entity)) {
                auto& background = manager.getComponent<BackgroundComponent>(entity);

                background.offsetX += background.scrollSpeed * dt;
                if (background.offsetX >= background.sprite.getTexture()->getSize().x) {
                    background.offsetX = 0;
                }
                background.sprite.setPosition(-background.offsetX, 0);
            }
        }
    }
}
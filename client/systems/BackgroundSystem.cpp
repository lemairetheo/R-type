//
// Created by Jean-Baptiste  Azan on 07/12/2024.
//

#include "BackgroundSystem.hpp"


namespace rtype {
    void BackgroundSystem::update(EntityManager& manager, float dt) {
        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (manager.hasComponent<BackgroundComponent>(entity)) {
                auto& background = manager.getComponent<BackgroundComponent>(entity);

                background.offsetX -= background.scrollSpeed * dt;

                float textureWidth = background.sprite.getTexture()->getSize().x;
                float scale = background.sprite.getScale().x;
                float scaledWidth = textureWidth * scale;

                if (background.offsetX <= -scaledWidth)
                    background.offsetX += scaledWidth;

                for (int i = 0; i < 2; ++i) {
                    float xPos = (i * scaledWidth) + background.offsetX;
                    background.sprite.setPosition(xPos, 0);

                    window.draw(background.sprite);
                }
            }
        }
    }

}
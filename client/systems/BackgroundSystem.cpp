//
// Created by Jean-Baptiste  Azan on 07/12/2024.
//

#include "BackgroundSystem.hpp"


namespace rtype {
    // BackgroundSystem.cpp
    void BackgroundSystem::update(EntityManager& manager, float dt) {
        float windowWidth = 800.0f;  // Idéalement, passez ça en paramètre

        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (manager.hasComponent<BackgroundComponent>(entity)) {
                auto& background = manager.getComponent<BackgroundComponent>(entity);

                background.offsetX += background.scrollSpeed * dt;
                float textureWidth = background.sprite.getTexture()->getSize().x;
                int numCopies = static_cast<int>(windowWidth / textureWidth) + 2;
                if (background.offsetX > textureWidth) {
                    background.offsetX -= textureWidth;
                }

                // Dessiner plusieurs copies du fond
                for (int i = 0; i < numCopies; ++i) {
                    float xPos = i * textureWidth - background.offsetX;
                    background.sprite.setPosition(xPos, 0);
                    window.draw(background.sprite);
                }
            }
        }
    }
}
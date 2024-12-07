//
// Created by Jean-Baptiste  Azan on 07/12/2024.
//

#include "ResourceManager.hpp"

namespace rtype {
    /**
     * \brief Load a texture into the ResourceManager.
     * \param id The id of the texture.
     * \param path The path to the texture file.
     */
    void ResourceManager::loadTexture(const std::string& id, const std::string& path) {
        auto texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromFile(path)) {
            throw std::runtime_error("Failed to load texture: " + path);
        }
        textures[id] = texture;
    }
    /**
     * \brief Get a texture from the ResourceManager.
     * \param id The id of the texture to get.
     * \return A shared pointer to the texture.
     */
    std::shared_ptr<sf::Texture> ResourceManager::getTexture(const std::string& id) {
        return textures[id];
    }
}
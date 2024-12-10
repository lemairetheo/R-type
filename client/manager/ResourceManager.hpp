/**
* \file ResourceManager.hpp
 * \brief Defines the ResourceManager class for the R-type project.
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

namespace rtype {
    /**
     * \class ResourceManager
     * \brief Singleton class for managing resources (textures, sounds, etc.).
     */
    class ResourceManager {
    public:
        static ResourceManager& getInstance() {
            static ResourceManager instance;
            return instance;
        }
        void loadTexture(const std::string& id, const std::string& path);
        std::shared_ptr<sf::Texture> getTexture(const std::string& id);
    private:
        ResourceManager() = default;
        std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures;
    };
}
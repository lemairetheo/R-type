// clientECS/ResourceManager.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

namespace rtype {
    class ResourceManager {
    public:
        static ResourceManager& getInstance() {
            static ResourceManager instance;
            return instance;
        }

        void loadTexture(const std::string& id, const std::string& path) {
            auto texture = std::make_shared<sf::Texture>();
            if (!texture->loadFromFile(path)) {
                throw std::runtime_error("Failed to load texture: " + path);
            }
            textures[id] = texture;
        }

        std::shared_ptr<sf::Texture> getTexture(const std::string& id) {
            return textures[id];
        }

    private:
        ResourceManager() = default;
        std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures;
    };
}
//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#include "Game.hpp"

namespace rtype {
    Game::Game() : window(sf::VideoMode(800, 600), "R-Type"), network(4242) {
        std::cout << "Game: Initializing..." << std::endl;

        auto& resources = ResourceManager::getInstance();
        resources.loadTexture("player", "assets/sprites/ship.gif");
        resources.loadTexture("background", "assets/sprites/r-typesheet1.gif");
        EntityID bg1 = entities.createEntity();
        {
            BackgroundComponent bgComp;
            bgComp.scrollSpeed = 30.0f;
            bgComp.layer = 0;
            bgComp.sprite.setTexture(*resources.getTexture("background"));
            bgComp.sprite.setTextureRect(sf::IntRect(0, 0, 800, 600));  // Ajustez selon vos besoins
            bgComp.sprite.setColor(sf::Color(20, 20, 50));  // Bleu très foncé
            entities.addComponent(bg1, bgComp);
        }
        EntityID bg2 = entities.createEntity();
        {
            BackgroundComponent bgComp;
            bgComp.scrollSpeed = 50.0f;
            bgComp.layer = 1;
            bgComp.sprite.setTexture(*resources.getTexture("background"));
            bgComp.sprite.setTextureRect(sf::IntRect(0, 100, 800, 50));  // Ligne des cercles
            entities.addComponent(bg2, bgComp);
        }
        systems.push_back(std::make_unique<BackgroundSystem>(window));
        systems.push_back(std::make_unique<MovementSystem>());
        systems.push_back(std::make_unique<AnimationSystem>());
        systems.push_back(std::make_unique<RenderSystem>(window));

        network.setMessageCallback([this](const std::vector<uint8_t>& data, const sockaddr_in& sender) {
            handleNetworkMessage(data, sender);
        });

        std::cout << "Game: Initialization complete" << std::endl;
    }

    void Game::handleNetworkMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender) {
        if (data.size() < sizeof(network::PacketHeader)) {
            return;
        }
        const auto* header = reinterpret_cast<const network::PacketHeader*>(data.data());
        if (header->type == static_cast<uint8_t>(network::PacketType::ENTITY_UPDATE)) {
            const auto* entityUpdate = reinterpret_cast<const network::EntityUpdatePacket*>(
                data.data() + sizeof(network::PacketHeader));
            EntityID entity = entityUpdate->entityId;
            if (!entities.hasComponent<Position>(entity)) {
                entities.createEntity();
                entities.addComponent(entity, Position{entityUpdate->x, entityUpdate->y});
                entities.addComponent(entity, Velocity{entityUpdate->dx, entityUpdate->dy});
                RenderComponent renderComp;
                renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("player"));
                renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 33, 17));
                renderComp.sprite.setOrigin(16.5f, 8.5f);
                entities.addComponent(entity, renderComp);
            } else {
                auto& pos = entities.getComponent<Position>(entity);
                auto& vel = entities.getComponent<Velocity>(entity);
                pos.x = entityUpdate->x;
                pos.y = entityUpdate->y;
                vel.dx = entityUpdate->dx;
                vel.dy = entityUpdate->dy;
            }
        }
    }

    void Game::run() {
        this->network.start();
        while (this->window.isOpen()) {
            handleEvents();
            update();
            render();
        }
        this->network.stop();
    }


    void Game::handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        InputComponent input;
        input.up = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
        input.down = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        input.left = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
        input.right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

        if (input.up || input.down || input.left || input.right) {
            std::vector<uint8_t> packet(sizeof(network::PacketHeader) + sizeof(network::PlayerInputPacket));
            auto* header = reinterpret_cast<network::PacketHeader*>(packet.data());
            auto* inputPacket = reinterpret_cast<network::PlayerInputPacket*>(packet.data() + sizeof(network::PacketHeader));
            header->magic[0] = 'R';
            header->magic[1] = 'T';
            header->version = 1;
            header->type = static_cast<uint8_t>(network::PacketType::PLAYER_INPUT);
            header->length = packet.size();
            header->sequence = 0;
            inputPacket->up = input.up;
            inputPacket->down = input.down;
            inputPacket->left = input.left;
            inputPacket->right = input.right;
            network.sendTo(packet);
        }
    }

    void Game::update() {
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastUpdate).count();
        lastUpdate = currentTime;

        for (auto& system : systems) {
            system->update(entities, dt);
        }
    }

    void Game::render() {
        window.clear();
        for (auto& system : systems) {
            system->update(entities, 0);
        }
        window.display();
    }
}
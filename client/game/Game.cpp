//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#include "Game.hpp"

namespace rtype {
    Game::Game() : window(sf::VideoMode(800, 600), "R-Type"), network(4242) {
        std::cout << "Game: Initializing..." << std::endl;

        auto& resources = ResourceManager::getInstance();
        resources.loadTexture("bg-blue", "assets/background/bg-blue.png");
        resources.loadTexture("bg-stars", "assets/background/bg-stars.png");
        resources.loadTexture("player", "assets/sprites/ship.gif");
        resources.loadTexture("sheet", "assets/sprites/r-typesheet1.gif");
        resources.loadTexture("enemy", "assets/sprites/r-typesheet7.gif");
        {
            EntityID bgDeep = entities.createEntity();
            BackgroundComponent bgComp;
            bgComp.scrollSpeed = 20.0f;
            bgComp.layer = 0;
            bgComp.sprite.setTexture(*resources.getTexture("bg-blue"));
            auto textureSize = bgComp.sprite.getTexture()->getSize();
            bgComp.sprite.setScale(800.0f / textureSize.x,600.0f / textureSize.y);
            entities.addComponent(bgDeep, bgComp);
            if (!font.loadFromFile("assets/fonts/Roboto-Medium.ttf")) {
                std::cerr << "Error loading font" << std::endl;
            }
            endGameText.setFont(font);
            endGameText.setString("GG Bro");
            endGameText.setCharacterSize(50);
            endGameText.setFillColor(sf::Color::White);
            endGameText.setStyle(sf::Text::Bold);
            endGameText.setPosition(200, 250);
        }
        {
            EntityID bgStars = entities.createEntity();
            BackgroundComponent bgComp;
            bgComp.scrollSpeed = 40.0f;
            bgComp.layer = 1;
            bgComp.sprite.setTexture(*resources.getTexture("bg-stars"));
            auto textureSize = bgComp.sprite.getTexture()->getSize();
            bgComp.sprite.setScale(800.0f / textureSize.x,600.0f / textureSize.y);
            bgComp.sprite.setColor(sf::Color(255, 255, 255, 180));
            entities.addComponent(bgStars, bgComp);
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
        if (data.size() < sizeof(network::PacketHeader)) return;

        const auto* header = reinterpret_cast<const network::PacketHeader*>(data.data());

        switch(static_cast<network::PacketType>(header->type)) {
            case network::PacketType::CONNECT_RESPONSE: {
                const auto* response = reinterpret_cast<const network::ConnectResponsePacket*>(data.data() + sizeof(network::PacketHeader));
                if (response->success) {
                    myPlayerId = response->playerId;
                    std::cout << "Game: Connected with ID " << myPlayerId << std::endl;
                }
                break;
            }
            case network::PacketType::ENTITY_DEATH: {
                const auto* response = reinterpret_cast<const network::EntityUpdatePacket*>(data.data() + sizeof(network::PacketHeader));
                if (response->entityId) {
                    entities.getComponents<Enemy>().erase(response->entityId);
                    entities.destroyEntity(response->entityId);
                }
                if (response->entityId2) {
                    entities.getComponents<Projectile>().erase(response->entityId2);
                    entities.destroyEntity(response->entityId2);
                }
                break;
            }
            case network::PacketType::ENTITY_UPDATE: {
                const auto* entityUpdate = reinterpret_cast<const network::EntityUpdatePacket*>(data.data() + sizeof(network::PacketHeader));
                EntityID entity = entityUpdate->entityId;
                if (!entities.hasComponent<Position>(entity)) {
                    std::cout << "Game: Entity doesn't exist : " << entityUpdate->type << std::endl;
                    entities.createEntity();
                    entities.addComponent(entity, Position{entityUpdate->x, entityUpdate->y});
                    entities.addComponent(entity, Velocity{entityUpdate->dx, entityUpdate->dy});
                    RenderComponent renderComp;
                    if (entityUpdate->type == 0) {
                        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("player"));
                        renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 33, 17));
                        renderComp.sprite.setOrigin(16.5f, 8.5f);
                    } else if (entityUpdate->type == 1) {
                        entities.addComponent(entity, Projectile{10.0f, true});
                        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("sheet"));
                        renderComp.sprite.setTextureRect(sf::IntRect(232, 58, 16, 16));
                        renderComp.sprite.setOrigin(8.0f, 8.0f);
                    } else if (entityUpdate->type == 2) {
                        entities.addComponent(entity, Enemy{1, true});
                        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("enemy"));
                        renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 34, 35));
                        renderComp.sprite.setOrigin(8.0f, 8.0f);
                    }
                    entities.addComponent(entity, renderComp);
                } else {
                    auto& pos = entities.getComponent<Position>(entity);
                    auto& vel = entities.getComponent<Velocity>(entity);
                    pos.x = entityUpdate->x;
                    pos.y = entityUpdate->y;
                    vel.dx = entityUpdate->dx;
                    vel.dy = entityUpdate->dy;
                }
                break;
            }
            case network::PacketType::END_GAME_STATE: {
                endGame = true;
                std::cout << "Game: End game state received. You won!" << std::endl;
                break;
            }
            default:
                break;
        }
    }

    void Game::run() {
        network.start();
        std::vector<uint8_t> connectPacket(sizeof(network::PacketHeader));
        auto* header = reinterpret_cast<network::PacketHeader*>(connectPacket.data());
        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(network::PacketType::CONNECT_REQUEST);
        header->length = connectPacket.size();
        header->sequence = 0;
        network.sendTo(connectPacket);
        std::cout << "Game: Sending connection request..." << std::endl;
        auto startTime = std::chrono::steady_clock::now();
        while (!myPlayerId) {
            auto currentTime = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count() > 5) {
                std::cout << "Game: Connection timeout" << std::endl;
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Game: Successfully connected with ID: " << myPlayerId << std::endl;
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
        std::vector<uint8_t> disconnectPacket(sizeof(network::PacketHeader));
        header = reinterpret_cast<network::PacketHeader*>(disconnectPacket.data());
        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(network::PacketType::DISCONNECT);
        header->length = disconnectPacket.size();
        header->sequence = 0;

        network.sendTo(disconnectPacket);
        network.stop();
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
        input.space = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

        if (input.up || input.down || input.left || input.right || input.space) {
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
            inputPacket->space = input.space;
            network.sendTo(packet);

            if (input.space) {
                inputPacket->space = false;
            }

        }
    }

    void Game::update() {
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastUpdate).count();
        lastUpdate = currentTime;

        std::cout << "Number of systems: " << systems.size() << std::endl;

        for (size_t i = 0; i < systems.size(); ++i) {
            try {
                std::cout << "Updating system " << i << std::endl;
                systems[i]->update(entities, dt);
            } catch (const std::exception& e) {
                std::cerr << "Exception in system " << i << ": " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Unknown exception in system " << i << std::endl;
            }
        }
    }

    void Game::render() {
        window.clear();

        if (!endGame) {
            for (auto& system : systems) {
                system->update(entities, 0);
            }
        }

        if (endGame) {
            window.draw(endGameText);
        }
        window.display();
    }
}
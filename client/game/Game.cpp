//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#include "Game.hpp"

namespace rtype {
    Game::Game() : window(sf::VideoMode(800, 600), "R-Type"), network(4242), menu(800,600) {
        std::cout << "Game: Initializing..." << std::endl;
        lifeText.setFont(font);
        lifeText.setCharacterSize(20);
        lifeText.setFillColor(sf::Color::White);
        lifeText.setPosition(10, 10);
        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 40); // En dessous de la vie
        scoreText.setString("Score: 0");
        levelText.setFont(font);
        levelText.setCharacterSize(20);
        levelText.setFillColor(sf::Color::White);
        levelText.setPosition(700, 10); // En dessous du score
        levelText.setString("Level: 1");

        auto& resources = ResourceManager::getInstance();
        resources.loadTexture("bg-blue", "assets/background/bg-blue.png");
        resources.loadTexture("bg-stars", "assets/background/bg-stars.png");
        resources.loadTexture("player", "assets/sprites/ship.gif");
        resources.loadTexture("healthPack", "assets/sprites/heal.png");
        resources.loadTexture("sheet", "assets/sprites/r-typesheet1.gif");
        resources.loadTexture("ultimate", "assets/sprites/r-typesheet2.gif");
        resources.loadTexture("enemy_lvl_1", "assets/sprites/r-typesheet7.gif");
        resources.loadTexture("enemy_lvl_2", "assets/sprites/r-typesheet9.gif");
        resources.loadTexture("enemy_lvl_3", "assets/sprites/r-typesheet14.gif");
        resources.loadTexture("enemy-colorblind", "assets/sprites/r-typesheet7-2.png");
        resources.loadTexture("sheet-colorblind", "assets/sprites/r-typesheet1-2.png");
        resources.loadTexture("bg-colorblind", "assets/background/Nebula Red.png");
        resources.loadTexture("player-colorblind", "assets/sprites/ship2.png");
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
        network.setMessageCallback([this](const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender) {
            handleNetworkMessage(data, sender);
        });
        std::cout << "Game: Initialization complete" << std::endl;
    }



    void Game::handleNetworkMessage(const std::vector<uint8_t>& data, [[maybe_unused]] const asio::ip::udp::endpoint& sender) {
    if (data.empty() || data.size() < sizeof(network::PacketHeader)) {
        std::cerr << "Invalid packet size" << std::endl;
        return;
    }

    const auto* header = reinterpret_cast<const network::PacketHeader*>(data.data());
    if (!header) {
        std::cerr << "Null header" << std::endl;
        return;
    }

    try {
        switch(static_cast<network::PacketType>(header->type)) {
            case network::PacketType::CONNECT_RESPONSE: {
                if (data.size() < sizeof(network::PacketHeader) + sizeof(network::ConnectResponsePacket)) {
                    std::cerr << "Packet too small for ConnectResponse" << std::endl;
                    return;
                }
                const auto* response = reinterpret_cast<const network::ConnectResponsePacket*>(data.data() + sizeof(network::PacketHeader));
                if (response->success) {
                    myPlayerId = response->playerId;
                    std::cout << "Game: Connected with ID " << myPlayerId << std::endl;
                }
                break;
            }

            case network::PacketType::ENTITY_DEATH: {
                if (data.size() < sizeof(network::PacketHeader) + sizeof(network::EntityUpdatePacket)) {
                    std::cerr << "Packet too small for EntityDeath" << std::endl;
                    return;
                }
                const auto* response = reinterpret_cast<const network::EntityUpdatePacket*>(data.data() + sizeof(network::PacketHeader));

                if (static_cast<int32_t>(response->entityId) != -1) {
                    std::cout << "Processing death for entity: " << response->entityId << std::endl;
                    try {
                        if (entities.hasComponent<Enemy>(response->entityId)) {
                            entities.getComponents<Enemy>().erase(response->entityId);
                        }
                        if (entities.hasComponent<Player>(response->entityId)) {
                            entities.getComponents<Player>().erase(response->entityId);
                        }
                        if (entities.hasComponent<Position>(response->entityId)) {
                            entities.getComponents<Position>().erase(response->entityId);
                        }
                        if (entities.hasComponent<Velocity>(response->entityId)) {
                            entities.getComponents<Velocity>().erase(response->entityId);
                        }
                        entities.destroyEntity(response->entityId);
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing entity death: " << e.what() << std::endl;
                    }
                }

                if (static_cast<int32_t>(response->entityId2) != -1) {
                    try {
                        if (entities.hasComponent<Projectile>(response->entityId2)) {
                            entities.getComponents<Projectile>().erase(response->entityId2);
                        }
                        entities.destroyEntity(response->entityId2);
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing projectile death: " << e.what() << std::endl;
                    }
                }
                break;
            }

            case network::PacketType::ENTITY_UPDATE: {
                if (data.size() < sizeof(network::PacketHeader) + sizeof(network::EntityUpdatePacket)) {
                    std::cerr << "Packet too small for EntityUpdate" << std::endl;
                    return;
                }
                const auto* entityUpdate = reinterpret_cast<const network::EntityUpdatePacket*>(data.data() + sizeof(network::PacketHeader));
                EntityID entity = entityUpdate->entityId;

                std::cout << "Processing update for entity: " << entity << " type: " << entityUpdate->type << std::endl;

                if (entityUpdate->type == 0 && entity == myPlayerId) {
                    playerScore = entityUpdate->score;
                    currentLevel = entityUpdate->level;
                    playerLife = entityUpdate->life;
                    lifeText.setString("Life: " + std::to_string(playerLife));
                    scoreText.setString("Score: " + std::to_string(playerScore));
                    levelText.setString("Level: " + std::to_string(currentLevel));
                }

                try {
                    if (!entities.hasComponent<Position>(entity)) {
                        std::cout << "Creating new entity: " << entity << std::endl;
                        entities.createEntity();
                        entities.addComponent(entity, Position{entityUpdate->x, entityUpdate->y});
                        entities.addComponent(entity, Velocity{entityUpdate->dx, entityUpdate->dy});
                        RenderComponent renderComp;

                        if (entityUpdate->type == 0) {
                            std::cout << "Creating player entity" << std::endl;
                            if (menu.getColorblindMode() == true)
                                renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("player-colorblind"));
                            else
                                renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("player"));
                            renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 33, 17));
                            renderComp.sprite.setOrigin(16.5f, 8.5f);
                        }
                        else if (entityUpdate->type == 1) {
                            std::cout << "Creating projectile entity" << std::endl;
                            entities.addComponent(entity, Projectile{10.0f, true, true, false});
                            if (menu.getColorblindMode() == true) {
                                renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("sheet-colorblind"));
                            } else {
                                renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("sheet"));
                            }
                            renderComp.sprite.setTextureRect(sf::IntRect(232, 58, 16, 16));
                            renderComp.sprite.setOrigin(8.0f, 8.0f);
                        }
                        else if (entityUpdate->type == 5) {
                            std::cout << "Creating ultimate projectile entity" << std::endl;
                            entities.addComponent(entity, Projectile{10.0f, true, true, true});
                            renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("ultimate"));
                            renderComp.sprite.setTextureRect(sf::IntRect(168, 342, 37, 31));
                            renderComp.sprite.setOrigin(8.0f, 8.0f);
                            renderComp.frameWidth = 37;
                            renderComp.frameHeight = 31;
                            renderComp.frameCount = 3;
                        }
                        else if (entityUpdate->type == 6) {
                            std::cout << "Creating health bonus entity" << std::endl;
                            entities.addComponent(entity, HealthBonus{3});
                            renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("healthPack"));
                            renderComp.sprite.setTextureRect(sf::IntRect(7, 11, 130, 130));
                            renderComp.sprite.setOrigin(8.0f, 8.0f);
                            renderComp.frameWidth = 130;
                            renderComp.frameHeight = 130;
                            renderComp.frameCount = 1;
                            renderComp.sprite.setScale(0.3f, 0.3f);
                        }
                        else if (entityUpdate->type >= 2 && entityUpdate->type <= 4) {
                            std::cout << "Creating enemy entity type: " << entityUpdate->type << std::endl;
                            static const std::unordered_map<int, std::string> textureMap = {
                                {2, "enemy_lvl_1"},
                                {3, "enemy_lvl_2"},
                                {4, "enemy_lvl_3"}
                            };

                            entities.addComponent(entity, Enemy{1, true, 1, 1.0f});

                            auto it = textureMap.find(entityUpdate->type);
                            if (it != textureMap.end()) {
                                renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture(it->second));
                                if (it->first == 2) {
                                    renderComp.frameWidth = 33;
                                    renderComp.frameHeight = 34;
                                    renderComp.Y = 0;
                                    renderComp.frameCount = 3;
                                    renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 36, 34));
                                }
                                else if (it->first == 3) {
                                    renderComp.frameWidth = 53;
                                    renderComp.frameHeight = 54;
                                    renderComp.Y = 0;
                                    renderComp.frameCount = 3;
                                    renderComp.sprite.setTextureRect(sf::IntRect(33, 1, 34, 28));
                                }
                                else if (it->first == 4) {
                                    renderComp.frameWidth = 49;
                                    renderComp.frameHeight = 52;
                                    renderComp.Y = 0;
                                    renderComp.frameCount = 3;
                                    renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 49, 52));
                                }
                                renderComp.sprite.setOrigin(renderComp.frameWidth / 2.0f, renderComp.frameHeight / 2.0f);
                            }
                        }

                        entities.addComponent(entity, renderComp);
                    }
                    else {
                        if (entityUpdate->type == 1 && entities.hasComponent<Enemy>(entity)) {
                            std::cout << "Converting enemy to projectile" << std::endl;
                            if (entities.hasComponent<RenderComponent>(entity)) {
                                auto& renderComp = entities.getComponent<RenderComponent>(entity);
                                entities.getComponents<Enemy>().erase(entity);
                                entities.addComponent(entity, Projectile{10.0f, true, true, false});
                                renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("sheet"));
                                renderComp.sprite.setTextureRect(sf::IntRect(232, 58, 16, 16));
                                renderComp.sprite.setOrigin(8.0f, 8.0f);
                            }
                        }

                        auto& pos = entities.getComponent<Position>(entity);
                        auto& vel = entities.getComponent<Velocity>(entity);
                        pos.x = entityUpdate->x;
                        pos.y = entityUpdate->y;
                        vel.dx = entityUpdate->dx;
                        vel.dy = entityUpdate->dy;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error processing entity update: " << e.what() << std::endl;
                }
                break;
            }

            case network::PacketType::END_GAME_STATE: {
                endGame = true;
                std::cout << "Game: End game state received" << std::endl;
                break;
            }

            default: {
                std::cerr << "Unknown packet type: " << static_cast<int>(header->type) << std::endl;
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Global error in handleNetworkMessage: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error in handleNetworkMessage" << std::endl;
    }
}

    void Game::displayMenu() {
        while (!menu.getIsPlaying()) {
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    exit(0);
                }
            }

            window.clear();

            if (menu.getColorblindMode()) {
                auto& bgComponents = entities.getComponents<BackgroundComponent>();

                for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
                    if (bgComponents[entity].has_value() && bgComponents[entity]->layer == 0) {
                        auto& bgComp = bgComponents[entity].value();
                        bgComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("bg-colorblind"));
                        break;
                    }
                }
            } else {
                auto& bgComponents = entities.getComponents<BackgroundComponent>();

                for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
                    if (bgComponents[entity].has_value() && bgComponents[entity]->layer == 0) {
                        auto& bgComp = bgComponents[entity].value();
                        bgComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("bg-blue"));
                        break;
                    }
                }
            }

            update();
            for (auto& system : systems) {
                system->update(entities, 0);
            }
            menu.render(window, event);
            window.display();
        }
    }

    void Game::run() {
        displayMenu();
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
        if (menu.getRightMode() == true) {
            input.up = sf::Keyboard::isKeyPressed(sf::Keyboard::I);
            input.down = sf::Keyboard::isKeyPressed(sf::Keyboard::K);
            input.left = sf::Keyboard::isKeyPressed(sf::Keyboard::J);
            input.right = sf::Keyboard::isKeyPressed(sf::Keyboard::L);
            input.space = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
            input.Ultimate = sf::Keyboard::isKeyPressed(sf::Keyboard::N);
        } else {
            input.up = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
            input.down = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
            input.left = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
            input.right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
            input.space = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
            input.Ultimate = sf::Keyboard::isKeyPressed(sf::Keyboard::X);
        }

        if (input.up || input.down || input.left || input.right || input.space || input.Ultimate) {
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
            inputPacket->ultimate = input.Ultimate;
            network.sendTo(packet);

            if (input.space)
                inputPacket->space = false;

            if (input.Ultimate)
                inputPacket->ultimate = false;
        }
    }

    void Game::update() {
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastUpdate).count();
        lastUpdate = currentTime;

        for (size_t i = 0; i < systems.size(); ++i) {
            try {
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
            window.draw(lifeText);
            window.draw(scoreText);
            window.draw(levelText);
        }

        if (endGame) {
            window.draw(endGameText);
        }
        window.display();
    }
}
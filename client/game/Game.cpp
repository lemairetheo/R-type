//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#include "Game.hpp"

namespace rtype {
    Game::Game() : window(sf::VideoMode(800, 600), "R-Type"), network(4242), menu(800, 600) {
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
        gameOverText.setFont(font);
        gameOverText.setString("Game Over");
        gameOverText.setCharacterSize(50);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setStyle(sf::Text::Bold);
        sf::FloatRect textRect = gameOverText.getLocalBounds();
        gameOverText.setOrigin(textRect.left + textRect.width / 2.0f,
                               textRect.top + textRect.height / 2.0f);
        gameOverText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

        auto &resources = ResourceManager::getInstance();
        resources.loadTexture("player", "assets/sprites/ship.gif");
        resources.loadTexture("bg-blue", "assets/background/bg-blue.png");
        resources.loadTexture("bg-stars", "assets/background/bg-stars.png");
        resources.loadTexture("healthPack", "assets/sprites/heal.png");
        resources.loadTexture("wall", "assets/sprites/wall.jpg");
        resources.loadTexture("sheet", "assets/sprites/r-typesheet1.gif");
        resources.loadTexture("ultimate", "assets/sprites/r-typesheet2.gif");
        resources.loadTexture("enemy_lvl_1", "assets/sprites/r-typesheet7.gif");
        resources.loadTexture("enemy_lvl_2", "assets/sprites/r-typesheet9.gif");
        resources.loadTexture("enemy_lvl_3", "assets/sprites/r-typesheet14.gif"); {
        resources.loadTexture("enemy_lvl_3", "assets/sprites/r-typesheet14.gif");
        resources.loadTexture("bg-colorblind", "assets/background/Nebula Red.png");
        resources.loadTexture("sheet-colorblind", "assets/sprites/r-typesheet1-2.png");
        resources.loadTexture("enemy_lvl_1-colorblind", "assets/sprites/r-typesheet7-2.png");
        resources.loadTexture("player-colorblind", "assets/sprites/ship2.png");
        resources.loadTexture("ultimate-colorblind", "assets/sprites/r-typesheet2-2.png");

            {
                EntityID bgDeep = entities.createEntity();
                BackgroundComponent bgComp;
                bgComp.scrollSpeed = 20.0f;
                bgComp.layer = 0;
                bgComp.sprite.setTexture(*resources.getTexture("bg-blue"));
                auto textureSize = bgComp.sprite.getTexture()->getSize();
                bgComp.sprite.setScale(800.0f / textureSize.x, 600.0f / textureSize.y);
                entities.addComponent(bgDeep, bgComp);
                if (!font.loadFromFile("assets/fonts/Roboto-Medium.ttf")) {
                    std::cerr << "Error loading font" << std::endl;
                }
            } {
                EntityID bgStars = entities.createEntity();
                BackgroundComponent bgComp;
                bgComp.scrollSpeed = 40.0f;
                bgComp.layer = 1;
                bgComp.sprite.setTexture(*resources.getTexture("bg-stars"));
                auto textureSize = bgComp.sprite.getTexture()->getSize();
                bgComp.sprite.setScale(800.0f / textureSize.x, 600.0f / textureSize.y);
                bgComp.sprite.setColor(sf::Color(255, 255, 255, 180));
                entities.addComponent(bgStars, bgComp);
            } {
                if (!musicGame.openFromFile("assets/music/415384_Nyan.mp3")) {
                    std::cerr << "Error loading music" << std::endl;
                } else {
                    musicGame.setLoop(true);
                    musicGame.setVolume(50.0f);
                    musicGame.play();
                }
            }
            systems.push_back(std::make_unique<BackgroundSystem>(window));
            systems.push_back(std::make_unique<MovementSystem>());
            systems.push_back(std::make_unique<AnimationSystem>());
            systems.push_back(std::make_unique<RenderSystem>(window));
            network.setMessageCallback([this](const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &sender) {
                handleNetworkMessage(data, sender);
            });
            std::cout << "Game: Initialization complete" << std::endl;
        }
    }
    void Game::handleNetworkMessage(const std::vector<uint8_t> &data,
                                    [[maybe_unused]] const asio::ip::udp::endpoint &sender) {
        if (data.size() < sizeof(network::PacketHeader)) return;

        const auto *header = reinterpret_cast<const network::PacketHeader *>(data.data());

        switch (static_cast<network::PacketType>(header->type)) {
            case network::PacketType::CONNECT_RESPONSE: {
                const auto *response = reinterpret_cast<const network::ConnectResponsePacket *>(
                    data.data() + sizeof(network::PacketHeader));
                if (response->success) {
                    myPlayerId = response->playerId;
                    std::cout << "Game: Connected with ID " << myPlayerId << std::endl;
                }
                break;
            }
            case network::PacketType::ENTITY_DEATH: {
                const auto *response = reinterpret_cast<const network::EntityUpdatePacket *>(
                    data.data() + sizeof(network::PacketHeader));

                if (playerLife <= 0)
                    playerIsDead = true;
                if (response->entityId != -1) {
                    if (entities.hasComponent<Enemy>(response->entityId))
                        entities.getComponents<Enemy>().erase(response->entityId);
                    if (entities.hasComponent<Player>(response->entityId))
                        entities.getComponents<Player>().erase(response->entityId);
                    if (entities.hasComponent<Position>(response->entityId))
                        entities.getComponents<Position>().erase(response->entityId);
                    if (entities.hasComponent<Velocity>(response->entityId))
                        entities.getComponents<Velocity>().erase(response->entityId);
                    entities.destroyEntity(response->entityId);
                }
                if (response->entityId2 != -1) {
                    entities.getComponents<Projectile>().erase(response->entityId2);
                    entities.destroyEntity(response->entityId2);
                }
                break;
            }
            case network::PacketType::ENTITY_UPDATE: {
                const auto *entityUpdate = reinterpret_cast<const network::EntityUpdatePacket *>(
                    data.data() + sizeof(network::PacketHeader));
                EntityID entity = entityUpdate->entityId;

                if (entityUpdate->type == 0 && entity == myPlayerId) {
                    playerScore = entityUpdate->score;
                    currentLevel = entityUpdate->level;
                    playerLife = entityUpdate->life;
                    lifeText.setString("Life: " + std::to_string(playerLife));
                    scoreText.setString("Score: " + std::to_string(playerScore));
                    levelText.setString("Level: " + std::to_string(currentLevel));
                }

                if (!entities.hasComponent<Position>(entity)) {
                    entities.createEntity();
                    entities.addComponent(entity, Position{entityUpdate->x, entityUpdate->y});
                    entities.addComponent(entity, Velocity{entityUpdate->dx, entityUpdate->dy});
                    RenderComponent renderComp;
                    if (entityUpdate->type == 0) {
                        std::cout << "player created" << std::endl;
                        if (menu.getColorblindMode() == true)
                            renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("player-colorblind"));
                        else
                            renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("player"));
                        renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 33, 17));
                        renderComp.sprite.setOrigin(16.5f, 8.5f);
                    } else if (entityUpdate->type == 1) {
                        entities.addComponent(entity, Projectile{10.0f, true, false, false});
                        if (menu.getColorblindMode() == true)
                            renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("sheet-colorblind"));
                        else
                            renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("sheet"));
                        renderComp.sprite.setTextureRect(sf::IntRect(232, 58, 16, 16));
                        renderComp.sprite.setOrigin(8.0f, 8.0f);
                    } else if (entityUpdate->type == 5) {
                        entities.addComponent(entity, Projectile{10.0f, true, false, false});
                        if (menu.getColorblindMode() == true)
                            renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("ultimate-colorblind"));
                        else
                            renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("ultimate"));
                        renderComp.sprite.setTextureRect(sf::IntRect(168, 342, 37, 31));
                        renderComp.sprite.setOrigin(8.0f, 8.0f);
                        renderComp.frameWidth = 37;
                        renderComp.frameHeight = 31;
                        renderComp.frameCount = 3;
                    } else if (entityUpdate->type == 6) {
                        entities.addComponent(entity, HealthBonus{3});
                        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("healthPack"));
                        renderComp.sprite.setTextureRect(sf::IntRect(7, 11, 130, 130));
                        renderComp.sprite.setOrigin(8.0f, 8.0f);
                        renderComp.frameWidth = 130;
                        renderComp.frameHeight = 130;
                        renderComp.frameCount = 1;
                        renderComp.sprite.setScale(0.3f, 0.3f);
                    } else if (entityUpdate->type >= 2 && entityUpdate->type <= 4) {
                        static std::unordered_map<int, std::string> textureMap;
                        if (menu.getColorblindMode() == true) {
                            textureMap = {
                                {2, "enemy_lvl_1-colorblind"},
                                {3, "enemy_lvl_2"},
                                {4, "enemy_lvl_3"}
                            };
                        } else {
                            textureMap = {
                                {2, "enemy_lvl_1"},
                                {3, "enemy_lvl_2"},
                                {4, "enemy_lvl_3"}
                            };
                        }

                        entities.addComponent(entity, Enemy{1, true, false, false});

                        auto it = textureMap.find(entityUpdate->type);
                        if (it != textureMap.end()) {
                            renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture(it->second));
                            if (it->first == 2) {
                                renderComp.frameWidth = 33;
                                renderComp.frameHeight = 34;
                                renderComp.Y = 0;
                                renderComp.frameCount = 3;
                                renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 36, 34));
                            } else if (it->first == 3) {
                                renderComp.frameWidth = 53;
                                renderComp.frameHeight = 54;
                                renderComp.Y = 0;
                                renderComp.frameCount = 3;
                                renderComp.sprite.setTextureRect(sf::IntRect(33, 1, 34, 28));
                            } else if (it->first == 4) {
                                renderComp.frameWidth = 49;
                                renderComp.frameHeight = 52;
                                renderComp.Y = 0;
                                renderComp.frameCount = 3;
                                renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 49, 52));
                            }
                            renderComp.sprite.setOrigin(renderComp.frameWidth / 2.0f, renderComp.frameHeight / 2.0f);
                        }
                    } else if (entityUpdate->type == 7) {
                        entities.addComponent(entity, Wall{3});
                        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("wall"));
                        renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 167, 587));
                        renderComp.sprite.setOrigin(10.0f, 8.0f);
                        renderComp.frameWidth = 165;
                        renderComp.frameHeight = 590;
                        renderComp.frameCount = 1;
                        renderComp.sprite.setScale(0.1f, 0.1f);
                    }

                    entities.addComponent(entity, renderComp);
                } else {
                    if (entityUpdate->type == 1 && entities.hasComponent<Enemy>(entity)) {
                        entities.getComponents<Enemy>().erase(entity);
                        entities.getComponents<Position>().erase(entity);
                        RenderComponent renderComp = entities.getComponent<RenderComponent>(entity);
                        entities.addComponent(entity, Projectile{10.0f, true, false, false});
                        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("sheet"));
                        renderComp.sprite.setTextureRect(sf::IntRect(232, 58, 16, 16));
                        renderComp.sprite.setOrigin(8.0f, 8.0f);
                        std::cout << entities.hasComponent<Enemy>(entity) << std::endl;
                    }
                    auto &pos = entities.getComponent<Position>(entity);
                    auto &vel = entities.getComponent<Velocity>(entity);
                    // if (entityUpdate->type == 0)
                    // std::cout << "player update posx" << pos.x << "pos y" << pos.y << std::endl;

                    pos.x = entityUpdate->x;
                    pos.y = entityUpdate->y;
                    vel.dx = entityUpdate->dx;
                    vel.dy = entityUpdate->dy;
                }
                break;
            }
            case network::PacketType::END_GAME_STATE: {
                playerIsDead = true;
                gameOverText.setString("Ta Gagner PD");
                std::cout << "Game: End game state received. You won!" << std::endl;
                break;
            }
            default:
                break;
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
        auto *header = reinterpret_cast<network::PacketHeader *>(connectPacket.data());
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
        header = reinterpret_cast<network::PacketHeader *>(disconnectPacket.data());
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
        if (menu.getRightMode()) {
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
            auto *header = reinterpret_cast<network::PacketHeader *>(packet.data());
            auto *inputPacket = reinterpret_cast<network::PlayerInputPacket *>(
                packet.data() + sizeof(network::PacketHeader));
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
            } catch (const std::exception &e) {
                std::cerr << "Exception in system " << i << ": " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Unknown exception in system " << i << std::endl;
            }
        }
    }

    void Game::render() {
        window.clear();

        if (!endGame && !playerIsDead) {
            for (auto &system: systems) {
                system->update(entities, 0);
            }
            window.draw(lifeText);
            window.draw(scoreText);
            window.draw(levelText);
        } else if (playerIsDead) {
            window.draw(gameOverText);
        }
        window.display();
    }
}

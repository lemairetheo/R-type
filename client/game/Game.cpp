//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#include "Game.hpp"

namespace rtype {
    Game::Game() : window(sf::VideoMode(800, 600), "R-Type"), network(nullptr), menu(800, 600) {
        if (!font.loadFromFile("assets/fonts/Roboto-Medium.ttf")) {
            std::cerr << "Error loading font" << std::endl;
            throw std::runtime_error("Failed to load font");
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
                if (response->success)
                    myPlayerId = response->playerId;
                break;
            }
            case network::PacketType::ENTITY_DEATH: {
                const auto *response = reinterpret_cast<const network::EntityUpdatePacket *>(
                    data.data() + sizeof(network::PacketHeader));

                if (playerLife <= 0)
                    playerIsDead = true;
                if (response->entityId != static_cast<uint32_t>(-1)) {
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
                if (response->entityId2 != static_cast<uint32_t>(-1)) {
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
                    }
                    auto &pos = entities.getComponent<Position>(entity);
                    auto &vel = entities.getComponent<Velocity>(entity);
                    pos.x = entityUpdate->x;
                    pos.y = entityUpdate->y;
                    vel.dx = entityUpdate->dx;
                    vel.dy = entityUpdate->dy;
                }
                break;
            }
            case network::PacketType::END_GAME_STATE: {
                currentState = GameState::VICTORY;
                gameOverText.setString("Victory!");
                gameOverText.setFillColor(sf::Color::Green);
                musicGame.stop();
                std::cout << "Game: Victory achieved!" << std::endl;
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
        bool retry = true;
        while (retry && window.isOpen()) {
            try {
                currentState = GameState::MENU;
                initMenuBackground();
                displayMenu();
                if (!window.isOpen()) return;
                systems.clear();
                entities.reset();
                currentState = GameState::CONNECTING;
                initGame();
                network->start();
                std::vector<uint8_t> connectPacket(sizeof(network::PacketHeader) + sizeof(network::ConnectRequestPacket));
                auto* header = reinterpret_cast<network::PacketHeader*>(connectPacket.data());
                auto* request = reinterpret_cast<network::ConnectRequestPacket*>(connectPacket.data() + sizeof(network::PacketHeader));
                header->magic[0] = 'R';
                header->magic[1] = 'T';
                header->version = 1;
                header->type = static_cast<uint8_t>(network::PacketType::CONNECT_REQUEST);
                header->length = connectPacket.size();
                header->sequence = 0;
                std::string username = menu.getUsername();
                std::strncpy(request->username, username.c_str(), sizeof(request->username) - 1);
                request->username[sizeof(request->username) - 1] = '\0';
                network->sendTo(connectPacket);
                std::cout << "Attempting to connect to " << menu.getServerIP() << ":" << menu.getServerPort() << std::endl;
                auto startTime = std::chrono::steady_clock::now();
                while (!myPlayerId && currentState == GameState::CONNECTING) {
                    auto currentTime = std::chrono::steady_clock::now();
                    if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count() > 5) {
                        throw std::runtime_error("Connection timeout");
                    }
                    window.clear();
                    sf::Text connectingText;
                    connectingText.setFont(font);
                    connectingText.setString("Connecting...");
                    connectingText.setCharacterSize(30);
                    connectingText.setFillColor(sf::Color::White);
                    connectingText.setPosition(window.getSize().x / 2.0f - 100, window.getSize().y / 2.0f);
                    window.draw(connectingText);
                    window.display();

                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                createBackgroundEntities();
                currentState = GameState::PLAYING;
                retry = false;

                while (window.isOpen() && currentState == GameState::PLAYING && !playerIsDead) {
                    handleEvents();
                    update();
                    render();
                }

                // Phase de fin (victoire ou défaite)
                currentState = playerIsDead ? GameState::GAME_OVER : GameState::VICTORY;

                // Déconnexion propre
                if (network) {
                    std::vector<uint8_t> disconnectPacket(sizeof(network::PacketHeader));
                    header = reinterpret_cast<network::PacketHeader*>(disconnectPacket.data());
                    header->magic[0] = 'R';
                    header->magic[1] = 'T';
                    header->version = 1;
                    header->type = static_cast<uint8_t>(network::PacketType::DISCONNECT);
                    header->length = disconnectPacket.size();
                    header->sequence = 0;

                    network->sendTo(disconnectPacket);
                    network->stop();
                }

            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;

                if (network) {
                    network->stop();
                    network = nullptr;
                }

                // Affichage de l'erreur et options de retry
                sf::Text errorText;
                errorText.setFont(font);
                errorText.setString("Connection failed: " + std::string(e.what()) + "\nPress SPACE to retry or ESC to quit");
                errorText.setCharacterSize(20);
                errorText.setFillColor(sf::Color::Red);
                errorText.setPosition(window.getSize().x / 2.0f - 200, window.getSize().y / 2.0f);

                while (window.isOpen()) {
                    sf::Event event;
                    while (window.pollEvent(event)) {
                        if (event.type == sf::Event::Closed) {
                            window.close();
                            return;
                        }
                        if (event.type == sf::Event::KeyPressed) {
                            if (event.key.code == sf::Keyboard::Space) {
                                retry = true;
                                currentState = GameState::MENU;
                                goto retry_connection;
                            } else if (event.key.code == sf::Keyboard::Escape) {
                                window.close();
                                return;
                            }
                        }
                    }

                    window.clear();
                    window.draw(errorText);
                    window.display();
                }
            }
            retry_connection:;
        }
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
            if (input.space)
                weaponSong.play();
            network->sendTo(packet);

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
        switch (currentState) {
            case GameState::MENU:
                    for (size_t i = 0; i < systems.size(); ++i) {
                        if (dynamic_cast<BackgroundSystem*>(systems[i].get()) ||
                            dynamic_cast<RenderSystem*>(systems[i].get())) {
                            systems[i]->update(entities, dt);
                            }
                    }
            break;
            case GameState::PLAYING:
                    for (size_t i = 0; i < systems.size(); ++i) {
                        try {
                            systems[i]->update(entities, dt);
                        } catch (const std::exception& e) {
                            std::cerr << "Exception in system " << i << ": " << e.what() << std::endl;
                        } catch (...) {
                            std::cerr << "Unknown exception in system " << i << std::endl;
                        }
                    }
            break;
            case GameState::GAME_OVER:
            case GameState::VICTORY:
                if (auto renderSystem = dynamic_cast<RenderSystem*>(systems.back().get())) {
                    renderSystem->update(entities, dt);
                }
            break;
            case GameState::CONNECTING:
                    if (auto renderSystem = dynamic_cast<RenderSystem*>(systems.back().get())) {
                        renderSystem->update(entities, dt);
                    }
            break;
        }
    }

    void Game::render() {
        window.clear();
        if (currentState == GameState::PLAYING || currentState == GameState::MENU ||
            currentState == GameState::VICTORY || currentState == GameState::GAME_OVER) {
            for (auto& system : systems) {
                system->update(entities, 0);
            }
            } else if (currentState == GameState::CONNECTING) {
                if (auto renderSystem = dynamic_cast<RenderSystem*>(systems.back().get())) {
                    renderSystem->update(entities, 0);
                }
            }
        if (currentState == GameState::PLAYING) {
            window.draw(lifeText);
            window.draw(scoreText);
            window.draw(levelText);
        } else if (currentState == GameState::GAME_OVER) {
            window.draw(gameOverText);
        } else if (currentState == GameState::VICTORY) {
            window.draw(gameOverText);
            sf::Text finalScoreText;
            finalScoreText.setFont(font);
            finalScoreText.setString("Final Score: " + std::to_string(playerScore));
            finalScoreText.setCharacterSize(30);
            finalScoreText.setFillColor(sf::Color::White);
            finalScoreText.setPosition(window.getSize().x / 2.0f - 100,
                                     window.getSize().y / 2.0f + 50);
            window.draw(finalScoreText);
        }

        window.display();
    }

    void Game::initGame() {
        std::string serverIP = menu.getServerIP();
        uint16_t serverPort = menu.getServerPort();
        network = std::make_unique<network::NetworkClient>(serverIP, serverPort);
        network->setMessageCallback([this](const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender) {
            handleNetworkMessage(data, sender);
        });
        initGameTexts();
        loadResources();
        setupSystems();
        initAudio();
        createBackgroundEntities();
    }

    void Game::initGameTexts() {
        lifeText.setFont(font);
        lifeText.setCharacterSize(20);
        lifeText.setFillColor(sf::Color::White);
        lifeText.setPosition(10, 10);

        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 40);
        scoreText.setString("Score: 0");

        levelText.setFont(font);
        levelText.setCharacterSize(20);
        levelText.setFillColor(sf::Color::White);
        levelText.setPosition(700, 10);
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
    }

    void Game::loadResources() {
        auto& resources = ResourceManager::getInstance();
        resources.loadTexture("player", "assets/sprites/ship.gif");
        resources.loadTexture("bg-blue", "assets/background/bg-blue.png");
        resources.loadTexture("bg-stars", "assets/background/bg-stars.png");
        resources.loadTexture("healthPack", "assets/sprites/heal.png");
        resources.loadTexture("wall", "assets/sprites/wall.jpg");
        resources.loadTexture("sheet", "assets/sprites/r-typesheet1.gif");
        resources.loadTexture("ultimate", "assets/sprites/r-typesheet2.gif");
        resources.loadTexture("enemy_lvl_1", "assets/sprites/r-typesheet7.gif");
        resources.loadTexture("enemy_lvl_2", "assets/sprites/r-typesheet9.gif");
        resources.loadTexture("enemy_lvl_3", "assets/sprites/r-typesheet14.gif");
        resources.loadTexture("bg-colorblind", "assets/background/Nebula Red.png");
        resources.loadTexture("sheet-colorblind", "assets/sprites/r-typesheet1-2.png");
        resources.loadTexture("enemy_lvl_1-colorblind", "assets/sprites/r-typesheet7-2.png");
        resources.loadTexture("player-colorblind", "assets/sprites/ship2.png");
        resources.loadTexture("ultimate-colorblind", "assets/sprites/r-typesheet2-2.png");
    }

    void Game::setupSystems() {
        systems.push_back(std::make_unique<BackgroundSystem>(window));
        systems.push_back(std::make_unique<MovementSystem>());
        systems.push_back(std::make_unique<AnimationSystem>());
        systems.push_back(std::make_unique<RenderSystem>(window));
    }

    void Game::initAudio() {
        if (!musicGame.openFromFile("assets/audio/415384_Nyan.mp3")) {
            std::cerr << "Error loading music" << std::endl;
        } else {
            musicGame.setLoop(true);
            musicGame.setVolume(10.0f);
            musicGame.play();
        }

        if (!weaponSong.openFromFile("assets/audio/poum.mp3")) {
            std::cerr << "Error loading weapon sound" << std::endl;
        } else {
            weaponSong.setLoop(false);
            weaponSong.setVolume(50.0f);
        }
    }

    void Game::initMenuBackground() {
        auto& resources = ResourceManager::getInstance();
        resources.loadTexture("bg-blue", "assets/background/bg-blue.png");
        resources.loadTexture("bg-stars", "assets/background/bg-stars.png");
        resources.loadTexture("bg-colorblind", "assets/background/Nebula Red.png");
        {
            EntityID bgDeep = entities.createEntity();
            BackgroundComponent bgComp;
            bgComp.scrollSpeed = 20.0f;
            bgComp.layer = 0;
            bgComp.sprite.setTexture(*resources.getTexture("bg-blue"));
            auto textureSize = bgComp.sprite.getTexture()->getSize();
            bgComp.sprite.setScale(800.0f / textureSize.x, 600.0f / textureSize.y);
            entities.addComponent(bgDeep, bgComp);
        }

        {
            EntityID bgStars = entities.createEntity();
            BackgroundComponent bgComp;
            bgComp.scrollSpeed = 40.0f;
            bgComp.layer = 1;
            bgComp.sprite.setTexture(*resources.getTexture("bg-stars"));
            auto textureSize = bgComp.sprite.getTexture()->getSize();
            bgComp.sprite.setScale(800.0f / textureSize.x, 600.0f / textureSize.y);
            bgComp.sprite.setColor(sf::Color(255, 255, 255, 180));
            entities.addComponent(bgStars, bgComp);
        }
        systems.push_back(std::make_unique<BackgroundSystem>(window));
        systems.push_back(std::make_unique<RenderSystem>(window));
    }

    void Game::createBackgroundEntities() {
        {
            EntityID bgDeep = entities.createEntity();
            BackgroundComponent bgComp;
            bgComp.scrollSpeed = 20.0f;
            bgComp.layer = 0;
            if (menu.getColorblindMode()) {
                bgComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("bg-colorblind"));
            } else {
                bgComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("bg-blue"));
            }
            auto textureSize = bgComp.sprite.getTexture()->getSize();
            float scaleX = (800.0f + bgComp.scrollSpeed) / textureSize.x;
            float scaleY = 600.0f / textureSize.y;
            bgComp.sprite.setScale(scaleX, scaleY);

            bgComp.sprite.setPosition(0, 0);
            entities.addComponent(bgDeep, bgComp);

            // Créer un second sprite de fond pour le défilement continu
            EntityID bgDeep2 = entities.createEntity();
            BackgroundComponent bgComp2 = bgComp;  // Copier les propriétés
            bgComp2.sprite.setPosition(800.0f, 0);  // Positionner juste après le premier
            entities.addComponent(bgDeep2, bgComp2);
        }

        {
            // Étoiles (même principe)
            EntityID bgStars = entities.createEntity();
            BackgroundComponent bgComp;
            bgComp.scrollSpeed = 40.0f;
            bgComp.layer = 1;
            bgComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("bg-stars"));

            auto textureSize = bgComp.sprite.getTexture()->getSize();
            float scaleX = (800.0f + bgComp.scrollSpeed) / textureSize.x;
            float scaleY = 600.0f / textureSize.y;
            bgComp.sprite.setScale(scaleX, scaleY);
            bgComp.sprite.setPosition(0, 0);
            bgComp.sprite.setColor(sf::Color(255, 255, 255, 180));
            entities.addComponent(bgStars, bgComp);

            // Second sprite d'étoiles
            EntityID bgStars2 = entities.createEntity();
            BackgroundComponent bgComp2 = bgComp;
            bgComp2.sprite.setPosition(800.0f, 0);
            entities.addComponent(bgStars2, bgComp2);
        }
    }
}

//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#include "Game.hpp"

namespace rtype
{
    Game::Game() : window(sf::VideoMode(800, 600), "R-Type"), network(nullptr), menu(800, 600),
                   backToMenuButton(sf::Vector2f(400, 500), sf::Vector2f(200, 50), "Back to menu")
    {
        if (!font.loadFromFile("assets/fonts/Roboto-Medium.ttf"))
        {
            std::cerr << "Error loading font" << std::endl;
            throw std::runtime_error("Failed to load font");
        }
        initPacketHandlers();
    }

    void Game::handleNetworkMessage(const std::vector<uint8_t>& data,
                                    [[maybe_unused]] const asio::ip::udp::endpoint& sender)
    {
        if (data.size() < sizeof(network::PacketHeader)) return;

        const auto* header = reinterpret_cast<const network::PacketHeader*>(data.data());
        auto type = static_cast<network::PacketType>(header->type);

        auto it = packetHandlers.find(type);
        if (it != packetHandlers.end())
        {
            it->second(data, sizeof(network::PacketHeader));
        }
    }

    void Game::handleEntityUpdate(const std::vector<uint8_t>& data, size_t offset)
    {
        const auto* entityUpdate = reinterpret_cast<const network::EntityUpdatePacket*>(data.data() + offset);
        EntityID entity = entityUpdate->entityId;

        // Update player stats if this is our player
        if (entityUpdate->type == 0 && entity == myPlayerId)
        {
            playerScore = entityUpdate->score;
            currentLevel = entityUpdate->level;
            playerLife = entityUpdate->life;
            lifeText.setString("Life: " + std::to_string(playerLife));
            scoreText.setString("Score: " + std::to_string(playerScore));
            levelText.setString("Level: " + std::to_string(currentLevel));
        }

        try
        {
            if (!entities.hasComponent<Position>(entity))
            {
                if (entity >= MAX_ENTITIES)
                {
                    std::cerr << "Warning: Server sent entity ID " << entity << " which exceeds MAX_ENTITIES" <<
                        std::endl;
                    return;
                }

                entities.resetEntityComponents(entity);
                entities.addComponent(entity, Position{entityUpdate->x, entityUpdate->y});
                entities.addComponent(entity, Velocity{entityUpdate->dx, entityUpdate->dy});
                RenderComponent renderComp;

                switch (entityUpdate->type)
                {
                case 0:
                    {
                        // Player
                        std::string playerTexture;
                        if (entity == myPlayerId)
                        {
                            playerTexture = menu.getColorblindMode() ? "player-colorblind" : "player";
                        }
                        else
                        {
                            // For other players, use different sprites based on their player number
                            int playerNum = (entity % 3) + 1; // Use modulo to cycle through 1-3
                            playerTexture = "player" + std::to_string(playerNum);
                            if (menu.getColorblindMode())
                            {
                                playerTexture += "-colorblind";
                            }
                        }
                        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture(playerTexture));
                        renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 33, 17));
                        renderComp.sprite.setOrigin(16.5f, 8.5f);
                        break;
                    }
                case 1:
                    {
                        // Projectile
                        entities.addComponent(entity, Projectile{10.0f, true, false, false});
                        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture(
                            menu.getColorblindMode() ? "sheet-colorblind" : "sheet"
                        ));
                        renderComp.sprite.setTextureRect(sf::IntRect(232, 58, 16, 16));
                        renderComp.sprite.setOrigin(8.0f, 8.0f);
                        break;
                    }
                case 5:
                    {
                        // Ultimate
                        entities.addComponent(entity, Projectile{10.0f, true, false, false});
                        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture(
                            menu.getColorblindMode() ? "ultimate-colorblind" : "ultimate"
                        ));
                        renderComp.sprite.setTextureRect(sf::IntRect(168, 342, 37, 31));
                        renderComp.sprite.setOrigin(8.0f, 8.0f);
                        renderComp.frameWidth = 37;
                        renderComp.frameHeight = 31;
                        renderComp.frameCount = 3;
                        break;
                    }
                case 6:
                    {
                        // Health Pack
                        entities.addComponent(entity, HealthBonus{3});
                        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("healthPack"));
                        renderComp.sprite.setTextureRect(sf::IntRect(7, 11, 130, 130));
                        renderComp.sprite.setOrigin(8.0f, 8.0f);
                        renderComp.frameWidth = 130;
                        renderComp.frameHeight = 130;
                        renderComp.frameCount = 1;
                        renderComp.sprite.setScale(0.3f, 0.3f);
                        break;
                    }
                case 2:
                case 3:
                case 4:
                    {
                        // Enemies
                        setupEnemyRenderComponent(entity, entityUpdate->type, renderComp);
                        break;
                    }
                case 7:
                    {
                        // Wall
                        setupWallRenderComponent(entity, renderComp);
                        break;
                    }
                case 8:
                    {
                        // Boss
                        entities.addComponent(entity, Enemy{3});
                        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("boss"));
                        renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 100, 34));
                        renderComp.sprite.setOrigin(15.0f, 23.0f);
                        renderComp.frameWidth = 33;
                        renderComp.frameHeight = 34;
                        renderComp.frameCount = 3;
                        renderComp.sprite.setScale(2.5f, 2.5f);
                        break;
                    }
                }
                entities.addComponent(entity, renderComp);
            }
            else
            {
                updateExistingEntity(entity, entityUpdate);
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error in handleEntityUpdate: " << e.what() <<
                " for entity " << entity << " of type " << entityUpdate->type << std::endl;
        }
    }

    void Game::handleBestScore(const std::vector<uint8_t>& data, size_t offset)
    {
        const auto* bestScore = reinterpret_cast<const network::BestScorePacket*>(data.data() + offset);
        playerStats.best_time = bestScore->best_time;
        playerStats.total_games = bestScore->games_won;
        playerStats.total_playtime = bestScore->total_playtime;
        playerStats.avg_score = bestScore->avg_score;
        updateStatsDisplay();
    }

    void Game::handleGameStats(const std::vector<uint8_t>& data, size_t offset)
    {
        const auto* gameStats = reinterpret_cast<const network::GameStatsPacket*>(data.data() + offset);
        playerStats.current_level = gameStats->current_level;
        playerStats.enemies_killed = gameStats->enemies_killed;
        currentLevel = gameStats->current_level;
        playerScore = gameStats->current_score;
        playerLife = gameStats->life_remaining;
        updateStatsDisplay();
    }

    void Game::handleEndGame(const std::vector<uint8_t>& data, size_t offset)
    {
        currentState = GameState::VICTORY;
        gameOverText.setString("Victory!");
        gameOverText.setFillColor(sf::Color::Green);
        displayFinalStats();
        musicGame.stop();
    }

    void Game::setupEnemyRenderComponent(EntityID entity, int type, RenderComponent& renderComp)
    {
        static std::unordered_map<int, std::string> textureMap;
        if (menu.getColorblindMode())
        {
            textureMap = {
                {2, "enemy_lvl_1-colorblind"},
                {3, "enemy_lvl_2"},
                {4, "enemy_lvl_3"}
            };
        }
        else
        {
            textureMap = {
                {2, "enemy_lvl_1"},
                {3, "enemy_lvl_2"},
                {4, "enemy_lvl_3"}
            };
        }
        entities.addComponent(entity, Enemy{1, true, false, false, false}); // dernier false pour isBoss
        auto it = textureMap.find(type);
        if (it != textureMap.end())
        {
            renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture(it->second));
            setupEnemyAnimation(type, renderComp);
        }
    }

    void Game::setupEnemyAnimation(int type, RenderComponent& renderComp)
    {
        if (type == 2)
        {
            renderComp.frameWidth = 33;
            renderComp.frameHeight = 34;
            renderComp.Y = 0;
            renderComp.frameCount = 3;
            renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 36, 34));
        }
        else if (type == 3)
        {
            renderComp.frameWidth = 53;
            renderComp.frameHeight = 54;
            renderComp.Y = 0;
            renderComp.frameCount = 3;
            renderComp.sprite.setTextureRect(sf::IntRect(33, 1, 34, 28));
        }
        else if (type == 4)
        {
            renderComp.frameWidth = 49;
            renderComp.frameHeight = 52;
            renderComp.Y = 0;
            renderComp.frameCount = 3;
            renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 49, 52));
        }
        renderComp.sprite.setOrigin(renderComp.frameWidth / 2.0f, renderComp.frameHeight / 2.0f);
    }

    void Game::setupWallRenderComponent(EntityID entity, RenderComponent& renderComp)
    {
        entities.addComponent(entity, Wall{3});
        renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("wall"));
        renderComp.sprite.setTextureRect(sf::IntRect(0, 0, 167, 587));
        renderComp.sprite.setOrigin(10.0f, 8.0f);
        renderComp.frameWidth = 165;
        renderComp.frameHeight = 590;
        renderComp.frameCount = 1;
        renderComp.sprite.setScale(0.1f, 0.1f);
    }

    void Game::updateExistingEntity(EntityID entity, const network::EntityUpdatePacket* entityUpdate)
    {
        if (entityUpdate->type == 1 && entities.hasComponent<Enemy>(entity))
        {
            entities.getComponents<Enemy>().erase(entity);
            entities.getComponents<Position>().erase(entity);
            RenderComponent renderComp = entities.getComponent<RenderComponent>(entity);
            entities.addComponent(entity, Projectile{10.0f, true, false, false});
            renderComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("sheet"));
            renderComp.sprite.setTextureRect(sf::IntRect(232, 58, 16, 16));
            renderComp.sprite.setOrigin(8.0f, 8.0f);
        }
        auto& pos = entities.getComponent<Position>(entity);
        auto& vel = entities.getComponent<Velocity>(entity);
        pos.x = entityUpdate->x;
        pos.y = entityUpdate->y;
        vel.dx = entityUpdate->dx;
        vel.dy = entityUpdate->dy;
    }

    void Game::handleConnectResponse(const std::vector<uint8_t>& data, size_t offset)
    {
        const auto* response = reinterpret_cast<const network::ConnectResponsePacket*>(data.data() + offset);
        if (response->success)
            myPlayerId = response->playerId;
    }

    void Game::handleEntityDeath(const std::vector<uint8_t>& data, size_t offset)
    {
        const auto* response = reinterpret_cast<const network::EntityUpdatePacket*>(data.data() + offset);

        if (playerLife <= 0)
            playerIsDead = true;

        if (response->entityId != static_cast<uint32_t>(-1))
        {
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

        if (response->entityId2 != static_cast<uint32_t>(-1))
        {
            entities.getComponents<Projectile>().erase(response->entityId2);
            entities.destroyEntity(response->entityId2);
        }
    }

    void Game::initPacketHandlers()
    {
        packetHandlers[network::PacketType::CONNECT_RESPONSE] =
            [this](const auto& data, size_t offset) { handleConnectResponse(data, offset); };

        packetHandlers[network::PacketType::ENTITY_DEATH] =
            [this](const auto& data, size_t offset) { handleEntityDeath(data, offset); };

        packetHandlers[network::PacketType::ENTITY_UPDATE] =
            [this](const auto& data, size_t offset) { handleEntityUpdate(data, offset); };

        packetHandlers[network::PacketType::BEST_SCORE] =
            [this](const auto& data, size_t offset) { handleBestScore(data, offset); };

        packetHandlers[network::PacketType::GAME_STATS] =
            [this](const auto& data, size_t offset) { handleGameStats(data, offset); };

        packetHandlers[network::PacketType::END_GAME_STATE] =
            [this](const auto& data, size_t offset) { handleEndGame(data, offset); };
        packetHandlers[network::PacketType::LOOSE_GAME_STATE] =
            [this](const auto& data, size_t offset) { handleLose(data, offset); };
    }

    void Game::updateStatsDisplay()
    {
        lifeText.setString("Life: " + std::to_string(playerLife));
        scoreText.setString("Score: " + std::to_string(playerScore));
        levelText.setString("Level: " + std::to_string(currentLevel));
        statsText.setString("Enemies Killed: " + std::to_string(playerStats.enemies_killed));
        bestScoreText.setString("Best Time: " + std::to_string(playerStats.best_time) + "s");
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - gameStartTime).count();
        timeText.setString("Time: " + std::to_string(elapsed) + "s");
    }

    void Game::handleLose(const std::vector<uint8_t>& data, size_t offset)
    {
        currentState = GameState::GAME_OVER;
        gameOverText.setString("You Lost!");
        gameOverText.setFillColor(sf::Color::Red);
        displayFinalStats();
        musicGame.stop();
    }


    void Game::displayFinalStats()
    {
        std::string statsStr =
            "Final Score: " + std::to_string(playerScore) + "\n" +
            "Level Reached: " + std::to_string(currentLevel) + "\n" +
            "Enemies Killed: " + std::to_string(playerStats.enemies_killed) + "\n" +
            "Best Time: " + std::to_string(playerStats.best_time) + "s\n" +
            "Total Games: " + std::to_string(playerStats.total_games);

        sf::Text statsText = UiHelpers::createText(
            statsStr,
            font,
            sf::Color::White,
            {400, 400},
            20,
            sf::Text::Bold
        );
        sf::FloatRect textRect = statsText.getLocalBounds();
        statsText.setOrigin(textRect.width / 2.0f, textRect.height / 2.0f);
        endGameText = statsText;
    }

    void Game::displayMenu()
    {
        while (!menu.getIsPlaying())
        {
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                    exit(0);
                }
            }
            window.clear();
            if (menu.getColorblindMode())
            {
                auto& bgComponents = entities.getComponents<BackgroundComponent>();

                for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity)
                {
                    if (bgComponents[entity].has_value() && bgComponents[entity]->layer == 0)
                    {
                        auto& bgComp = bgComponents[entity].value();
                        bgComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("bg-colorblind"));
                        break;
                    }
                }
            }
            else
            {
                auto& bgComponents = entities.getComponents<BackgroundComponent>();
                for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity)
                {
                    if (bgComponents[entity].has_value() && bgComponents[entity]->layer == 0)
                    {
                        auto& bgComp = bgComponents[entity].value();
                        bgComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("bg-blue"));
                        break;
                    }
                }
            }
            update();
            for (auto& system : systems)
            {
                system->update(entities, 0);
            }
            menu.render(window, event);
            window.display();
        }
    }

    void Game::run()
    {
        bool retry = true;
        while (retry && window.isOpen())
        {
            try
            {
                currentState = GameState::MENU;
                initMenuBackground();
                displayMenu();
                if (!window.isOpen()) return;
                systems.clear();
                entities.reset();
                currentState = GameState::CONNECTING;
                initGame();
                network->start();
                std::vector<uint8_t> connectPacket(
                    sizeof(network::PacketHeader) + sizeof(network::ConnectRequestPacket));
                auto* header = reinterpret_cast<network::PacketHeader*>(connectPacket.data());
                auto* request = reinterpret_cast<network::ConnectRequestPacket*>(connectPacket.data() + sizeof(
                    network::PacketHeader));
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
                std::cout << "Attempting to connect to " << menu.getServerIP() << ":" << menu.getServerPort() <<
                    std::endl;
                auto startTime = std::chrono::steady_clock::now();
                while (!myPlayerId && currentState == GameState::CONNECTING)
                {
                    auto currentTime = std::chrono::steady_clock::now();
                    if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count() > 5)
                    {
                        throw std::runtime_error("Connection timeout");
                    }
                    window.clear();
                    sf::Text connectingText = UiHelpers::createText("Connecting to server...", font, sf::Color::White,
                                                                    {10, 10}, 20, sf::Text::Bold);
                    window.draw(connectingText);
                    window.display();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                createBackgroundEntities();
                currentState = GameState::PLAYING;
                retry = false;
                while (window.isOpen() && currentState == GameState::PLAYING && !playerIsDead)
                {
                    handleEvents();
                    update();
                    render();
                    updateStatsDisplay();
                }
                currentState = playerIsDead ? GameState::GAME_OVER : GameState::VICTORY;
                while (window.isOpen() && (currentState == GameState::VICTORY || currentState == GameState::GAME_OVER ||
                    currentState == GameState::LEADERBOARD))
                {
                    sf::Event evt;
                    while (window.pollEvent(evt))
                    {
                        if (evt.type == sf::Event::Closed)
                        {
                            // Gérer la déconnexion proprement avant de fermer
                            if (network)
                            {
                                std::vector<uint8_t> disconnectPacket(sizeof(network::PacketHeader));
                                auto* header = reinterpret_cast<network::PacketHeader*>(disconnectPacket.data());
                                header->magic[0] = 'R';
                                header->magic[1] = 'T';
                                header->version = 1;
                                header->type = static_cast<uint8_t>(network::PacketType::DISCONNECT);
                                header->length = disconnectPacket.size();
                                header->sequence = 0;

                                network->sendTo(disconnectPacket);
                                network->stop();
                                network = nullptr;
                            }
                            window.close();
                        }
                        else if (evt.type == sf::Event::KeyPressed)
                        {
                            if (evt.key.code == sf::Keyboard::Space)
                            {
                                // Demander le leaderboard et passer à l'état leaderboard
                                currentState = GameState::LEADERBOARD;
                            }
                            else if (evt.key.code == sf::Keyboard::Escape)
                            {
                                // Gérer la déconnexion proprement avant de quitter
                                if (network)
                                {
                                    std::vector<uint8_t> disconnectPacket(sizeof(network::PacketHeader));
                                    auto* header = reinterpret_cast<network::PacketHeader*>(disconnectPacket.data());
                                    header->magic[0] = 'R';
                                    header->magic[1] = 'T';
                                    header->version = 1;
                                    header->type = static_cast<uint8_t>(network::PacketType::DISCONNECT);
                                    header->length = disconnectPacket.size();
                                    header->sequence = 0;

                                    network->sendTo(disconnectPacket);
                                    network->stop();
                                    network = nullptr;
                                }
                                window.close();
                            }
                        }
                    }

                    // Mettre à jour l'affichage
                    update();
                    render();
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error: " << e.what() << std::endl;

                if (network)
                {
                    network->stop();
                    network = nullptr;
                }
                sf::Text errorText = UiHelpers::createText(
                    "Error: " + std::string(e.what()) + "\nPress Space to retry or Escape to quit", font,
                    sf::Color::Red, {10, 10}, 20, sf::Text::Bold);
                while (window.isOpen())
                {
                    sf::Event event;
                    while (window.pollEvent(event))
                    {
                        if (event.type == sf::Event::Closed)
                        {
                            window.close();
                            return;
                        }
                        if (event.type == sf::Event::KeyPressed)
                        {
                            if (event.key.code == sf::Keyboard::Space)
                            {
                                retry = true;
                                currentState = GameState::MENU;
                                goto retry_connection;
                            }
                            else if (event.key.code == sf::Keyboard::Escape)
                            {
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

    void Game::handleEvents()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        InputComponent input;
        if (menu.getRightMode())
        {
            input.up = sf::Keyboard::isKeyPressed(sf::Keyboard::I);
            input.down = sf::Keyboard::isKeyPressed(sf::Keyboard::K);
            input.left = sf::Keyboard::isKeyPressed(sf::Keyboard::J);
            input.right = sf::Keyboard::isKeyPressed(sf::Keyboard::L);
            input.space = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
            input.Ultimate = sf::Keyboard::isKeyPressed(sf::Keyboard::N);
        }
        else
        {
            input.up = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
            input.down = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
            input.left = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
            input.right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
            input.space = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
            input.Ultimate = sf::Keyboard::isKeyPressed(sf::Keyboard::X);
        }

        if (input.up || input.down || input.left || input.right || input.space || input.Ultimate)
        {
            std::vector<uint8_t> packet(sizeof(network::PacketHeader) + sizeof(network::PlayerInputPacket));
            auto* header = reinterpret_cast<network::PacketHeader*>(packet.data());
            auto* inputPacket = reinterpret_cast<network::PlayerInputPacket*>(
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

    void Game::update()
    {
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastUpdate).count();
        lastUpdate = currentTime;
        if (currentState == GameState::PLAYING)
        {
            cleanupEntities();
        }
        switch (currentState)
        {
        case GameState::MENU:
            for (size_t i = 0; i < systems.size(); ++i)
            {
                if (dynamic_cast<BackgroundSystem*>(systems[i].get()) ||
                    dynamic_cast<RenderSystem*>(systems[i].get()))
                {
                    systems[i]->update(entities, dt);
                }
            }
            break;
        case GameState::PLAYING:
            for (size_t i = 0; i < systems.size(); ++i)
            {
                try
                {
                    systems[i]->update(entities, dt);
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Exception in system " << i << ": " << e.what() << std::endl;
                } catch (...)
                {
                    std::cerr << "Unknown exception in system " << i << std::endl;
                }
            }
            break;
        case GameState::GAME_OVER:
        case GameState::VICTORY:
            if (auto renderSystem = dynamic_cast<RenderSystem*>(systems.back().get()))
            {
                renderSystem->update(entities, dt);
            }
            break;
        case GameState::CONNECTING:
            if (auto renderSystem = dynamic_cast<RenderSystem*>(systems.back().get()))
            {
                renderSystem->update(entities, dt);
            }
        case GameState::LEADERBOARD:
            renderLeaderboard();
            break;
        }
    }

    void Game::render()
    {
        window.clear();
        if (currentState == GameState::PLAYING || currentState == GameState::MENU ||
            currentState == GameState::VICTORY || currentState == GameState::GAME_OVER)
        {
            for (auto& system : systems)
            {
                system->update(entities, 0);
            }
        }
        else if (currentState == GameState::CONNECTING)
        {
            if (auto renderSystem = dynamic_cast<RenderSystem*>(systems.back().get()))
            {
                renderSystem->update(entities, 0);
            }
        }
        if (currentState == GameState::PLAYING)
        {
            window.draw(lifeText);
            window.draw(scoreText);
            window.draw(levelText);
            window.draw(statsText);
            window.draw(bestScoreText);
            window.draw(timeText);
        }
        else if (currentState == GameState::GAME_OVER)
        {
            window.draw(gameOverText);
            if (playerIsDead)
            {
                sf::Text finalScoreText = UiHelpers::createText("Better luck next time!", font, sf::Color::White,
                                                                {400, 400}, 20, sf::Text::Bold);
                window.draw(finalScoreText);
            }
        }
        else if (currentState == GameState::VICTORY)
        {
            window.draw(gameOverText);
            sf::Text finalScoreText = UiHelpers::createText("Final score: " + std::to_string(playerScore), font,
                                                            sf::Color::White, {400, 350}, 20, sf::Text::Bold);
            window.draw(finalScoreText);
            window.draw(endGameText);
        }
        else if (currentState == GameState::LEADERBOARD)
        {
            renderLeaderboard();
        }
        window.display();
    }

    void Game::initGame()
    {
        std::string serverIP = menu.getServerIP();
        uint16_t serverPort = menu.getServerPort();
        network = std::make_unique<network::NetworkClient>(serverIP, serverPort);
        network->setMessageCallback([this](const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender)
        {
            handleNetworkMessage(data, sender);
        });
        initGameTexts();
        loadResources();
        setupSystems();
        initAudio();
        createBackgroundEntities();
    }

    void Game::initGameTexts()
    {
        lifeText = UiHelpers::createText("Life: 3", font, sf::Color::White, {10, 10}, 20, sf::Text::Bold);
        scoreText = UiHelpers::createText("Score: 0", font, sf::Color::White, {10, 40}, 20, sf::Text::Bold);
        levelText = UiHelpers::createText("Level: 1", font, sf::Color::White, {10, 70}, 20, sf::Text::Bold);
        gameOverText = UiHelpers::createText("Game Over", font, sf::Color::Red, {400, 300}, 50, sf::Text::Bold);
        sf::FloatRect textRect = gameOverText.getLocalBounds();
        gameOverText.setOrigin(textRect.left + textRect.width / 2.0f,
                               textRect.top + textRect.height / 2.0f);
        gameOverText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
        statsText = UiHelpers::createText("Enemies Killed: 0", font, sf::Color::White, {10, 100}, 20, sf::Text::Bold);
        bestScoreText = UiHelpers::createText("Best Time: --", font, sf::Color::White, {10, 130}, 20, sf::Text::Bold);
        timeText = UiHelpers::createText("Time: 0s", font, sf::Color::White, {10, 160}, 20, sf::Text::Bold);
        gameStartTime = std::chrono::steady_clock::now();
    }

    void Game::loadResources()
    {
        auto& resources = ResourceManager::getInstance();
        resources.loadTexture("player", "assets/sprites/ship.gif");
        resources.loadTexture("player1", "assets/sprites/ship2.gif");
        resources.loadTexture("bg-blue", "assets/background/bg-blue.png");
        resources.loadTexture("bg-stars", "assets/background/bg-stars.png");
        resources.loadTexture("healthPack", "assets/sprites/heal.png");
        resources.loadTexture("wall", "assets/sprites/wall.jpg");
        resources.loadTexture("sheet", "assets/sprites/r-typesheet1.gif");
        resources.loadTexture("ultimate", "assets/sprites/r-typesheet2.gif");
        resources.loadTexture("enemy_lvl_1", "assets/sprites/r-typesheet7.gif");
        resources.loadTexture("enemy_lvl_2", "assets/sprites/r-typesheet9.gif");
        resources.loadTexture("enemy_lvl_3", "assets/sprites/r-typesheet14.gif");
        resources.loadTexture("boss", "assets/sprites/r-typesheet11.gif");
        resources.loadTexture("bg-colorblind", "assets/background/Nebula Red.png");
        resources.loadTexture("sheet-colorblind", "assets/sprites/r-typesheet1-2.png");
        resources.loadTexture("enemy_lvl_1-colorblind", "assets/sprites/r-typesheet7-2.png");
        resources.loadTexture("player-colorblind", "assets/sprites/ship2.png");
        resources.loadTexture("ultimate-colorblind", "assets/sprites/r-typesheet2-2.png");
    }

    void Game::setupSystems()
    {
        systems.push_back(std::make_unique<BackgroundSystem>(window));
        systems.push_back(std::make_unique<MovementSystem>());
        systems.push_back(std::make_unique<AnimationSystem>());
        systems.push_back(std::make_unique<RenderSystem>(window));
    }

    void Game::initAudio()
    {
        if (!musicGame.openFromFile("assets/audio/415384_Nyan.mp3"))
        {
            std::cerr << "Error loading music" << std::endl;
        }
        else
        {
            musicGame.setLoop(true);
            musicGame.setVolume(10.0f);
            musicGame.play();
        }

        if (!weaponSong.openFromFile("assets/audio/poum.mp3"))
        {
            std::cerr << "Error loading weapon sound" << std::endl;
        }
        else
        {
            weaponSong.setLoop(false);
            weaponSong.setVolume(50.0f);
        }
    }

    void Game::initMenuBackground()
    {
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

    void Game::createBackgroundEntities()
    {
        {
            EntityID bgDeep = entities.createEntity();
            BackgroundComponent bgComp;
            bgComp.scrollSpeed = 20.0f;
            bgComp.layer = 0;
            if (menu.getColorblindMode())
            {
                bgComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("bg-colorblind"));
            }
            else
            {
                bgComp.sprite.setTexture(*ResourceManager::getInstance().getTexture("bg-blue"));
            }
            auto textureSize = bgComp.sprite.getTexture()->getSize();
            float scaleX = (800.0f + bgComp.scrollSpeed) / textureSize.x;
            float scaleY = 600.0f / textureSize.y;
            bgComp.sprite.setScale(scaleX, scaleY);

            bgComp.sprite.setPosition(0, 0);
            entities.addComponent(bgDeep, bgComp);
            EntityID bgDeep2 = entities.createEntity();
            BackgroundComponent bgComp2 = bgComp;
            bgComp2.sprite.setPosition(800.0f, 0);
            entities.addComponent(bgDeep2, bgComp2);
        }
        {
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
            EntityID bgStars2 = entities.createEntity();
            BackgroundComponent bgComp2 = bgComp;
            bgComp2.sprite.setPosition(800.0f, 0);
            entities.addComponent(bgStars2, bgComp2);
        }
    }

    void Game::cleanupEntities()
    {
        static auto lastCleanup = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCleanup).count() >= 1)
        {
            lastCleanup = now;
            auto positions = entities.getEntitiesWithComponents<Position>();
            for (EntityID entity : positions)
            {
                const auto& pos = entities.getComponent<Position>(entity);
                if (pos.x < -100 || pos.x > 900 || pos.y < -100 || pos.y > 700)
                {
                    if (entity != myPlayerId)
                    {
                        // Ne pas supprimer le joueur
                        entities.destroyEntity(entity);
                    }
                }
            }
        }
    }

    void Game::handleLeaderboard(const std::vector<uint8_t>& data, size_t offset)
    {
        const auto* leaderboard = reinterpret_cast<const network::LeaderboardPacket*>(data.data() + offset);
        leaderboardEntries.clear();

        for (size_t i = 0; i < leaderboard->nb_entries; ++i)
        {
            const auto& entry = leaderboard->entries[i];
            leaderboardEntries.push_back({
                std::string(entry.username),
                entry.score,
                entry.level_reached,
                entry.time
            });
        }
        currentState = GameState::LEADERBOARD;
    }

    void Game::renderLeaderboard()
    {
        sf::Text titleText = UiHelpers::createText(
            "LEADERBOARD",
            font,
            sf::Color::White,
            {window.getSize().x / 2.0f, 50},
            30,
            sf::Text::Bold
        );
        titleText.setOrigin(titleText.getLocalBounds().width / 2.0f, 0);

        float yPos = 120;
        window.draw(titleText);

        for (size_t i = 0; i < leaderboardEntries.size(); ++i)
        {
            const auto& entry = leaderboardEntries[i];
            std::string text = std::to_string(i + 1) + ". " + entry.username +
                " - Score: " + std::to_string(entry.score) +
                " - Level: " + std::to_string(entry.level_reached) +
                " - Time: " + std::to_string(entry.time) + "s";

            sf::Text entryText = UiHelpers::createText(
                text,
                font,
                sf::Color::White,
                {100, yPos},
                20,
                sf::Text::Regular
            );
            window.draw(entryText);
            yPos += 40;
        }
        if (backToMenuButton.handleEvent(event, window))
        {
            currentState = GameState::MENU;
        }
        backToMenuButton.render(window, "normal");
    }
}

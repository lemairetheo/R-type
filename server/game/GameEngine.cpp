// server/game/GameEngine.cpp
#include "GameEngine.hpp"

namespace rtype::game {

    GameEngine::GameEngine(network::NetworkManager& networkManager)
        : network(networkManager),
          lastUpdate(std::chrono::steady_clock::now())
    {
        try {
            dbManager = std::make_unique<database::DatabaseManager>("rtype_scores.db");
            scoreRepository = std::make_unique<database::ScoreRepository>(*dbManager);
            userRepository = std::make_unique<database::UserRepository>(*dbManager);
            EntityID playerEntity = entities.createEntity();
            entities.addComponent(playerEntity, Position{400.0f, 300.0f});
            entities.addComponent(playerEntity, Velocity{0.0f, 0.0f});
            systems.push_back(std::make_unique<MovementSystem>());
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize: " << e.what() << std::endl;
            throw;
        }
    }

    void GameEngine::initializeLevel() {
        auto enemies = entities.getEntitiesWithComponents<Enemy>();
        for (EntityID enemy : enemies) {
            entities.destroyEntity(enemy);
        }
        enemySpawnQueue.clear();
        currentLevel = 1;
        spawnEnemiesForLevel(currentLevel);
    }

    void GameEngine::broadcastWorldState() {
        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (!entities.hasComponent<Position>(entity) || !entities.hasComponent<Velocity>(entity))
                continue;

            const auto& pos = entities.getComponent<Position>(entity);
            const auto& vel = entities.getComponent<Velocity>(entity);

            int type = 0;
            int life = 0;
            int score = 0;

            if (entities.hasComponent<Player>(entity)) {
                const auto& player = entities.getComponent<Player>(entity);
                type = 0;
                life = player.life;
                score = player.score;
            } else if (entities.hasComponent<Projectile>(entity) && !entities.hasComponent<Enemy>(entity)) {
                type = entities.getComponent<Projectile>(entity).isUltimate ? 5 : 1;
            } else if (entities.hasComponent<Enemy>(entity)) {
                if (!entities.getComponent<Enemy>(entity).isBoss)
                    type = entities.hasTypeEnemy<Enemy>(entity);
                else
                    type = 8;
            } else if (entities.hasComponent<HealthBonus>(entity)) {
                type = 6;
            } else if (entities.hasComponent<Wall>(entity)) {
                type = 7;
            }

            network.broadcast(network.createEntityUpdatePacket(entity, type, pos, vel, life, score, currentLevel));
        }
    }

    EntityID GameEngine::createNewPlayer(const asio::ip::udp::endpoint& sender) {
        std::string clientId = sender.address().to_string() + ":" + std::to_string(sender.port());
        EntityID playerEntity = entities.createEntity();
        spawnEnemiesForLevel(1);
        entities.addComponent(playerEntity, Position{400.0f, 300.0f});
        entities.addComponent(playerEntity, Velocity{0.0f, 0.0f});
        entities.addComponent(playerEntity, Player{0, 10, 0});
        entities.addComponent(playerEntity, InputComponent{});
        entities.addComponent(playerEntity, NetworkComponent{static_cast<uint32_t>(playerEntity)});
        playerEntities[clientId] = playerEntity;
        gameStartTimes[clientId] = std::chrono::steady_clock::now();
        return playerEntity;
    }

    void GameEngine::handleGameCompletion(const std::string& clientId) {
        auto gameStartIt = gameStartTimes.find(clientId);
        auto userIt = connectedUsers.find(clientId);
        if (gameStartIt != gameStartTimes.end() && userIt != connectedUsers.end()) {
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                endTime - gameStartIt->second).count();

            int enemiesKilled = 0;
            int score = 0;
            if (auto playerEntityIt = playerEntities.find(clientId);
                playerEntityIt != playerEntities.end()) {
                auto& player = entities.getComponent<Player>(playerEntityIt->second);
                score = player.score;
                enemiesKilled = score;
            }

            try {
                scoreRepository->updatePlayerScore(
                    userIt->second.username,
                    duration,
                    currentLevel,
                    enemiesKilled
                );
                userRepository->updateUserStats(userIt->second.username, duration);
                auto scorePacket = network.createScoreUpdatePacket(
                    userIt->second.username,
                    duration,
                    score
                );
                network.sendTo(scorePacket, network.getClientEndpoint(clientId));
                auto bestScore = scoreRepository->getPlayerBestScore(userIt->second.username);  // au lieu de userIt->second.id
                if (bestScore) {
                    auto bestScorePacket = network.createBestScorePacket(
                        userIt->second.username,
                        bestScore->score_time,
                        userIt->second.total_games_played
                    );
                    network.sendTo(bestScorePacket, network.getClientEndpoint(clientId));
                }
            } catch (const std::exception& e) {
                std::cerr << "Failed to update score: " << e.what() << std::endl;
            }
        }
    }

    bool checkCollision(const Position& pos1, float radius1, const Position& pos2, float radius2) {
        float dx = pos1.x - pos2.x;
        float dy = pos1.y - pos2.y;
        float distanceSquared = dx * dx + dy * dy;
        float radiusSum = radius1 + radius2;
        return distanceSquared <= (radiusSum * radiusSum);
    }

    void GameEngine::update() {
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastUpdate).count();
        lastUpdate = currentTime;

        handleHealthPackSpawns();
        handleEnemySpawns(dt);
        handleEnemyShoot();
        handleCollisions();
        handleWallSpawns();

        for (auto& system : systems) {
            system->update(entities, dt);
        }

        broadcastWorldState();
    }

    void GameEngine::handleHealthPackSpawns() {
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastUpdateHealthPack).count();

        if (dt >= 10.2f)
            lastUpdateHealthPack = currentTime;
        else
            return;
        if (entities.getEntitiesWithComponents<HealthBonus>().size() > 3) {
            lastUpdateHealthPack = currentTime;
            return;
        }
        float x = static_cast<float>(rand() % 760);
        float y = static_cast<float>(rand() % 560);
        auto walls = entities.getEntitiesWithComponents<Wall>();
        auto HealthPacks = entities.getEntitiesWithComponents<HealthBonus>();
        bool stopLoop = false;
        while (stopLoop != true) {
            for (EntityID wall : walls) {
                const auto& wallPos = entities.getComponent<Position>(wall);
                if (!checkCollisionRect({x, y}, 25, wallPos, 20, 60)) {
                    stopLoop = true;
                    break;
                }
            }
            for (EntityID HealthPack : HealthPacks) {
                const auto& HealthPackPos = entities.getComponent<Position>(HealthPack);
                if (!checkCollisionRect({x, y}, 25, HealthPackPos, 20, 60)) {
                    stopLoop = true;
                    break;
                }
            }
            if (stopLoop == false)
                break;
            x = static_cast<float>(rand() % 760);
            y = static_cast<float>(rand() % 560);
        }
        EntityID healthPackEntity = entities.createEntity();
        entities.addComponent(healthPackEntity, Position{x, y});
        entities.addComponent(healthPackEntity, HealthBonus{3});
        entities.addComponent(healthPackEntity, Velocity{0.0f, 0.0f});
    }

    void GameEngine::handleWallSpawns() {
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastUpdateWallShoot).count();

        if (dt >= 1.2f)
            lastUpdateWallShoot = currentTime;
        else
            return;
        spawnWall(250, 100);
        spawnWall(250, 450);
    }

    void GameEngine::handleEnemySpawns(float dt) {
        for (auto it = enemySpawnQueue.begin(); it != enemySpawnQueue.end();) {
            it->delay -= dt;
            if (it->delay <= 0) {
                spawnEnemy(it->x, it->y, it->level, it->isBoss);
                it = enemySpawnQueue.erase(it);
            } else {
                ++it;
            }
        }
    }

    void GameEngine::handleEnemyShoot() {
        auto currentTime = std::chrono::steady_clock::now();
        if ( std::chrono::duration<float>(currentTime - lastUpdateBossShoot).count() >= 1.9f) { // shoot boss
            auto enemies = entities.getEntitiesWithComponents<Enemy>();
            lastUpdateBossShoot = currentTime;

            for (EntityID enemy : enemies) {
                if (entities.getComponent<Enemy>(enemy).isBoss == true) {
                    shoot_system_.update(entities, enemy, true, 50);
                    shoot_system_.update(entities, enemy, true, 0);
                    shoot_system_.update(entities, enemy, true, -50);
                }
            }
        }

        float dt = std::chrono::duration<float>(currentTime - lastUpdateEnemiesShoot).count();

        if (dt >= 1.2f) { // shoot enemy
            lastUpdateEnemiesShoot = currentTime;
        } else {
            return;
        }

        auto enemies = entities.getEntitiesWithComponents<Enemy>();

        for (EntityID enemy : enemies) {
            shoot_system_.update(entities, enemy, false, 0);
        }
    }

    void GameEngine::handleCollisions() {
        auto missiles = entities.getEntitiesWithComponents<Projectile>();
        auto enemies = entities.getEntitiesWithComponents<Enemy>();
        auto walls = entities.getEntitiesWithComponents<Wall>();
        auto players = entities.getEntitiesWithComponents<Player>();
        auto healthPacks = entities.getEntitiesWithComponents<HealthBonus>();

        for (EntityID missile : missiles) {
            if (!entities.hasComponent<Position>(missile)) continue;

            const auto& missilePos = entities.getComponent<Position>(missile);
            const float missileRadius = 5.0f;

            // Handle collision with enemies
            for (EntityID enemy: enemies) {
                if (!entities.hasComponent<Position>(enemy)) continue;

                const auto& enemyPos = entities.getComponent<Position>(enemy);
                const float enemyRadius = 20.0f;

                if (checkCollision(missilePos, missileRadius, enemyPos, enemyRadius) && entities.getComponent<Projectile>(missile).lunchByType != 2) {
                    handleCollision(missile, enemy);
                }
            }
            for (EntityID player : players) {
                const auto& playerPos = entities.getComponent<Position>(player);

                if (checkCollision(missilePos, missileRadius, playerPos, 20.0f) && entities.getComponent<Projectile>(missile).lunchByType != 0) {
                    handleCollisionPlayer(missile, player);
                    break;
                }
            }
            for (EntityID wall : walls) {
                const auto& wallPos = entities.getComponent<Position>(wall);

                if (checkCollisionRect(missilePos, missileRadius, wallPos, 20.0f, 60.0f) ) {
                    handleCollisionPlayer(missile, wall);
                    break;
                }
            }
        }

        for (EntityID player : players) {
            const auto& playerPos = entities.getComponent<Position>(player);

            for (EntityID healthPack : healthPacks) {
                const auto& healthPackPos = entities.getComponent<Position>(healthPack);

                if (checkCollisionRect(playerPos, 20.0f, healthPackPos, 20.0f, 20.0f)) {
                    auto& playerComp = entities.getComponent<Player>(player);
                    playerComp.life += entities.getComponent<HealthBonus>(healthPack).healthAmount;
                    auto packet = network.createEntityDeathPacket(-1, healthPack);
                    network.broadcast(packet);
                    entities.destroyEntity(healthPack);
                }
            }
        }
    }

    void GameEngine::handleCollision(EntityID missile, EntityID enemy) {
        auto& projectile = entities.getComponent<Projectile>(missile);
        entities.getComponent<Enemy>(enemy).life -= projectile.damage;

        if (entities.getComponent<Enemy>(enemy).life <= 0) {
            updatePlayerScore();
            if (projectile.isUltimate) {
                auto packet = network.createEntityDeathPacket(-1, enemy);
                network.broadcast(packet);
                entities.destroyEntity(enemy);
            } else {
                auto packet = network.createEntityDeathPacket(missile, enemy);
                network.broadcast(packet);
                entities.destroyEntity(enemy);
                entities.destroyEntity(missile);
            }
        } else if (!projectile.isUltimate) {
            auto packet = network.createEntityDeathPacket(missile, -1);
            network.broadcast(packet);
            entities.destroyEntity(missile);
        }
    }

    void GameEngine::handleCollisionPlayer(EntityID missile, EntityID player) {
        entities.getComponent<Player>(player).life--;
        auto packet = network.createEntityDeathPacket(missile, -1);
        network.broadcast(packet);
        entities.destroyEntity(missile);

        if (entities.getComponent<Player>(player).life <= 0) {
            packet = network.createEntityDeathPacket(-1, player);
            network.broadcast(packet);
        }
    }

    std::tuple<float, int, float> GameEngine::getEnemyAttributes(int level) {
        auto it = enemyAttributes.find(level);
        if (it != enemyAttributes.end()) {
            return it->second;
        }

        return {10.0f, 5, -30.0}; // Default
    }

    void GameEngine::spawnEnemy(float x, float y, int level, bool isBoss) {
        EntityID enemyEntity = entities.createEntity();
        entities.addComponent(enemyEntity, Position{x, y});
        entities.addComponent(enemyEntity, Velocity{(isBoss ? -5.0f : -50.0f), 0.0f});

        int enemyLevel;
        float randValue = dis(gen);

        if (level == 1) {
            enemyLevel = 1;
        } else if (level == 2) {
            if (randValue <= 0.75f)
                enemyLevel = 1;
            else
                enemyLevel = 2;
        } else if (level == 3) {
            if (randValue <= 0.7f)
                enemyLevel = 1;
            else
                enemyLevel = 3;
        } else {
            enemyLevel = 1;
        }
        auto [life, damage, speedShoot] = getEnemyAttributes(enemyLevel);

        entities.addComponent(enemyEntity, Enemy{
                isBoss ? (damage * 3) : damage,
                isBoss ? (life * 5) : life,
                level,
                isBoss ? (speedShoot * 2) : speedShoot,
                isBoss
            }
        );
    }

    void GameEngine::spawnWall(float x, float y) {
        EntityID wallEntity = entities.createEntity();
        entities.addComponent(wallEntity, Position{x, y});
        entities.addComponent(wallEntity, Velocity{0.0f, 0.0f});

        entities.addComponent(wallEntity, Wall{1});
    }

    bool GameEngine::checkCollision(const Position& pos1, float radius1, const Position& pos2, float radius2) {
        float dx = pos1.x - pos2.x;
        float dy = pos1.y - pos2.y;
        float distanceSquared = dx * dx + dy * dy;
        float radiusSum = radius1 + radius2;
        return distanceSquared <= (radiusSum * radiusSum);
    }

    bool GameEngine::checkCollisionRect(const Position& circlePos, float radius, const Position& rectPos, float rectWidth, float rectHeight) {
        float closestX = std::max(rectPos.x, std::min(circlePos.x, rectPos.x + rectWidth));
        float closestY = std::max(rectPos.y, std::min(circlePos.y, rectPos.y + rectHeight));

        float dx = circlePos.x - closestX;
        float dy = circlePos.y - closestY;

        return (dx * dx + dy * dy) <= (radius * radius);
    }

    void GameEngine::updatePlayerScore() {
        for (EntityID entity : entities.getEntitiesWithComponents<Player>()) {
            auto& player = entities.getComponent<Player>(entity);
            player.score++;
            auto threshold = SCORE_THRESHOLDS.find(currentLevel);
            if (threshold != SCORE_THRESHOLDS.end() && player.score >= threshold->second) {
                if (currentLevel < 3) {
                    currentLevel++;
                    switchToNextLevel();
                } else {
                    broadcastEndGameState();
                }
            }
        }
    }

    void GameEngine::switchToNextLevel() {
        auto enemies = entities.getEntitiesWithComponents<Enemy>();
        for (EntityID enemy : enemies) {
            entities.destroyEntity(enemy);
        }

        enemySpawnQueue.clear();
        spawnEnemiesForLevel(currentLevel);
    }

    void GameEngine::spawnEnemiesForLevel(int level) {
        const int ENEMIES_PER_LEVEL[] = {15, 15, 15};
        int nbEnemies = ENEMIES_PER_LEVEL[level - 1];

        for (int i = 0; i < nbEnemies; i++) {
            float delay = static_cast<float>(i) * 2.0f;
            auto x = static_cast<float>(820);
            auto y = static_cast<float>(rand() % 560);
            enemySpawnQueue.push_back(PendingSpawn{delay, x, y, level, false});
        }
        enemySpawnQueue.push_back(PendingSpawn{static_cast<float>(nbEnemies) * 2.0f, static_cast<float>(820), static_cast<float>(rand() % 560), level, true});
    }

    void GameEngine::broadcastEndGameState() {
        for (const auto& [clientId, entityId] : playerEntities) {
            handleGameCompletion(clientId);
        }
        auto packet = network.createEndGamePacket();
        network.broadcast(packet);
    }


    void GameEngine::handleNetworkMessage(const std::vector<uint8_t>& data, [[maybe_unused]] const sockaddr_in& sender, const std::string& clientId) {
        if (data.size() < sizeof(network::PacketHeader)) return;

        const auto* header = reinterpret_cast<const network::PacketHeader*>(data.data());

        if (header->type == static_cast<uint8_t>(network::PacketType::PLAYER_INPUT)) {
            auto it = playerEntities.find(clientId);
            if (it == playerEntities.end()) return;

            EntityID playerEntity = it->second;
            if (entities.hasComponent<Position>(playerEntity) &&
                entities.hasComponent<Velocity>(playerEntity)) {
                auto& vel = entities.getComponent<Velocity>(playerEntity);
                auto& input = entities.getComponent<InputComponent>(playerEntity);
                const auto* inputPacket = reinterpret_cast<const network::PlayerInputPacket*>(
                    data.data() + sizeof(network::PacketHeader));
                if (inputPacket->space) {
                    input.space = true;
                }
                if (inputPacket->ultimate) {
                    input.Ultimate = true;
                }
                if (input.space) {
                    shoot_system_.update(entities, playerEntity, false, 0);
                    input.space = false;
                }
                if (input.Ultimate) {
                    shoot_system_.update(entities, playerEntity, true, 0);
                    input.Ultimate = false;
                }

                vel.dx = 0.0f;
                vel.dy = 0.0f;
                if (inputPacket->left) vel.dx = -speed;
                if (inputPacket->right) vel.dx = speed;
                if (inputPacket->up) vel.dy = -speed;
                if (inputPacket->down) vel.dy = speed;
            }
        }
        if (header->type == static_cast<uint8_t>(network::PacketType::CONNECT_REQUEST)) {
            const auto* connectRequest = reinterpret_cast<const network::ConnectRequestPacket*>(
                data.data() + sizeof(network::PacketHeader));
            std::string username(connectRequest->username);
            std::cerr << "Received connection request for username: '" << username << "'" << std::endl;

            try {
                auto user = userRepository->getUser(username);
                if (!user) {
                    std::cerr << "User not found, creating new user..." << std::endl;
                    user = userRepository->createUser(username);
                    if (!user) {
                        std::cerr << "Failed to create user" << std::endl;
                        throw std::runtime_error("Failed to create user");
                    }
                }
                std::cerr << "User successfully found/created: " << user->username << std::endl;

                playerUsernames[clientId] = username;
                connectedUsers[clientId] = *user;
                userRepository->updateLastConnection(username);
                std::cerr << "Last connection updated for user: " << username << std::endl;

                auto bestScore = scoreRepository->getPlayerBestScore(user->username);
                if (bestScore) {
                    std::cerr << "Found best score for user: " << bestScore->score_time << std::endl;
                    auto bestScorePacket = network.createBestScorePacket(
                        username,
                        bestScore->score_time,
                        user->total_games_played
                    );
                    network.sendTo(bestScorePacket, network.getClientEndpoint(clientId));
                } else {
                    std::cerr << "No best score found for user" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Failed to handle user connection: " << e.what() << std::endl;
                std::cerr << "User that caused error: " << username << std::endl;
            }
        }
    }

    void GameEngine::handleMessage(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender) {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(sender.port());
        addr.sin_addr.s_addr = sender.address().to_v4().to_ulong();
        std::string clientId = sender.address().to_string() + ":" + std::to_string(sender.port());
        handleNetworkMessage(data, addr, clientId);
    }

    void GameEngine::handlePlayerDisconnection(const std::string& clientId) {
        if (auto it = playerEntities.find(clientId); it != playerEntities.end()) {
            EntityID entityId = it->second;
            entities.destroyEntity(entityId);
            playerEntities.erase(it);
            std::cout << "Player " << clientId << " disconnected" << std::endl;
        }
    }
} // namespace rtype::game
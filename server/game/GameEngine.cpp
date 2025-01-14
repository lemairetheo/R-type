// server/game/GameEngine.cpp
#include "GameEngine.hpp"

namespace rtype::game {

    GameEngine::GameEngine(network::NetworkManager& networkManager)
        : network(networkManager),
    lastUpdate(std::chrono::steady_clock::now())
    {
        EntityID playerEntity = entities.createEntity();
        entities.addComponent(playerEntity, Position{400.0f, 300.0f});
        entities.addComponent(playerEntity, Velocity{0.0f, 0.0f});
        systems.push_back(std::make_unique<MovementSystem>());
        spawnEnemiesForLevel(1);
    }

    void GameEngine::broadcastWorldState() {
        auto walls = entities.getEntitiesWithComponents<Wall>();
        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (entities.hasComponent<Position>(entity) && entities.hasComponent<Velocity>(entity)) {
                const auto& pos = entities.getComponent<Position>(entity);
                const auto& vel = entities.getComponent<Velocity>(entity);

                std::vector<uint8_t> packet(sizeof(network::PacketHeader) + sizeof(network::EntityUpdatePacket));
                auto* header = reinterpret_cast<network::PacketHeader*>(packet.data());
                auto* update = reinterpret_cast<network::EntityUpdatePacket*>(packet.data() + sizeof(network::PacketHeader));
                header->magic[0] = 'R';
                header->magic[1] = 'T';
                header->version = 1;
                header->type = static_cast<uint8_t>(network::PacketType::ENTITY_UPDATE);
                header->length = packet.size();
                header->sequence = 0;
                update->entityId = entity;
                update->x = pos.x;
                update->y = pos.y;
                update->dx = vel.dx;
                update->dy = vel.dy;
                update->life = 0;
                update->score = 0;
                update->level = 0;

                if (entities.hasComponent<Player>(entity)) {
                    update->type = 0;
                    update->life = entities.getComponent<Player>(entity).life;
                    update->score = entities.getComponent<Player>(entity).score;
                    update->level = currentLevel;
                } if (entities.hasComponent<Projectile>(entity) && !entities.hasComponent<Enemy>(entity)) {
                    if (entities.getComponent<Projectile>(entity).isUltimate)
                        update->type = 5;
                    else
                        update->type = 1;
                } else if (entities.hasComponent<Enemy>(entity)) {
                    auto it = entities.hasTypeEnemy<Enemy>(entity);
                    update->type = it;
                } else if (entities.hasComponent<HealthBonus>(entity)) {
                    std::cout << "HealthBonus" << std::endl;
                    update->type = 6;
                } else if (entities.hasComponent<Wall>(entity)) {
                    update->type = 7;
                } else
                    update->type = 0;
                network.broadcast(packet);
            }
        }
    }

    EntityID GameEngine::createNewPlayer(const asio::ip::udp::endpoint& sender) {
        std::string clientId = sender.address().to_string() + ":" + std::to_string(sender.port());
        EntityID playerEntity = entities.createEntity();
        entities.addComponent(playerEntity, Position{400.0f, 300.0f});
        entities.addComponent(playerEntity, Velocity{0.0f, 0.0f});
        entities.addComponent(playerEntity, Player{0, 10, 0});
        entities.addComponent(playerEntity, InputComponent{});
        entities.addComponent(playerEntity, NetworkComponent{static_cast<uint32_t>(playerEntity)});
        playerEntities[clientId] = playerEntity;
        return playerEntity;
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

        static float spawnTimer = 0.0f;
        spawnTimer += dt;

        if (spawnTimer >= 10.0f) {
            spawnHealthPack();
            spawnTimer = 0.0f;
        }

        handleEnemySpawns(dt);
        handleEnemyShoot();
        handleCollisions();
        handleWallSpawns();

        for (auto& system : systems) {
            system->update(entities, dt);
        }

        broadcastWorldState();
    }

    void GameEngine::spawnHealthPack() {
        float x = static_cast<float>(rand() % 800); // Position X aléatoire
        float y = static_cast<float>(rand() % 600); // Position Y aléatoire

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
        spawnWall(250, 100, 1);
    }

    void GameEngine::handleEnemySpawns(float dt) {
        for (auto it = enemySpawnQueue.begin(); it != enemySpawnQueue.end();) {
            it->delay -= dt;
            if (it->delay <= 0) {
                spawnEnemy(it->x, it->y, it->level);
                it = enemySpawnQueue.erase(it);
            } else {
                ++it;
            }
        }
    }

    void GameEngine::handleEnemyShoot() {
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastUpdateEnemiesShoot).count();

        if (dt >= 1.2f)
            lastUpdateEnemiesShoot = currentTime;
        else
            return;

        auto enemies = entities.getEntitiesWithComponents<Enemy>();

        for (EntityID enemy : enemies) {
            shoot_system_.update(entities, enemy, false);
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
            // Handle collision with players
            for (EntityID player : players) {
                const auto& playerPos = entities.getComponent<Position>(player);

                if (checkCollision(missilePos, missileRadius, playerPos, 20.0f) && entities.getComponent<Projectile>(missile).lunchByType != 0) {
                    handleCollisionPlayer(missile, player);
                    break;
                }
            }
            // Handle collision with wall for all missiles
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

                if (checkCollision(playerPos, 20.0f, healthPackPos, 10.0f)) {
                    auto& playerComp = entities.getComponent<Player>(player);
                    playerComp.life += entities.getComponent<HealthBonus>(healthPack).healthAmount;

                    // Détruire le sprite de vie après la collision
                    auto packet = createEntityDeathPacket(-1, healthPack);
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
                auto packet = createEntityDeathPacket(-1, enemy);
                network.broadcast(packet);
                entities.destroyEntity(enemy);
            } else {
                auto packet = createEntityDeathPacket(missile, enemy);
                network.broadcast(packet);
                entities.destroyEntity(enemy);
                entities.destroyEntity(missile);
            }
        } else if (!projectile.isUltimate) {
            auto packet = createEntityDeathPacket(missile, -1);
            network.broadcast(packet);
            entities.destroyEntity(missile);
        }
    }

    void GameEngine::handleCollisionPlayer(EntityID missile, EntityID player) {
        entities.getComponent<Player>(player).life--;
        auto packet = createEntityDeathPacket(missile, -1);
        network.broadcast(packet);
        entities.destroyEntity(missile);

        if (entities.getComponent<Player>(player).life <= 0) {
            packet = createEntityDeathPacket(-1, player);
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

    void GameEngine::spawnEnemy(float x, float y, int level) {
        EntityID enemyEntity = entities.createEntity();
        entities.addComponent(enemyEntity, Position{x, y});
        entities.addComponent(enemyEntity, Velocity{-50.0f, 0.0f});

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

        entities.addComponent(enemyEntity, Enemy{damage, life, enemyLevel, speedShoot});
    }

    void GameEngine::spawnWall(float x, float y, int level) {
        EntityID wallEntity = entities.createEntity();
        entities.addComponent(wallEntity, Position{x, y});
        entities.addComponent(wallEntity, Velocity{0.0f, 0.0f});

        entities.addComponent(wallEntity, Wall{1});
    }
s
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


    std::vector<uint8_t> GameEngine::createEntityDeathPacket(EntityID missile, EntityID enemy) const {
        std::vector<uint8_t> packet(sizeof(network::PacketHeader) + sizeof(network::EntityUpdatePacket));
        auto* header = reinterpret_cast<network::PacketHeader*>(packet.data());
        auto* update = reinterpret_cast<network::EntityUpdatePacket*>(packet.data() + sizeof(network::PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(network::PacketType::ENTITY_DEATH);
        header->length = packet.size();
        header->sequence = 0;

        update->entityId = enemy;
        update->entityId2 = missile;
        update->type = 0;  // Vous pouvez ajuster ce type si nécessaire

        return packet;
    }

    void GameEngine::updatePlayerScore() {
        for (EntityID entity : entities.getEntitiesWithComponents<Player>()) {
            auto& player = entities.getComponent<Player>(entity);
            player.score++;

            // Vérification du changement de niveau
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
        // Suppression des ennemis existants
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
            auto x = static_cast<float>(800);
            auto y = static_cast<float>(rand() % 600);
            enemySpawnQueue.push_back(PendingSpawn{delay, x, y, level});
        }
    }

    void GameEngine::broadcastEndGameState() {
        auto packet = createEndGamePacket();
        network.broadcast(packet);
    }

    std::vector<uint8_t> GameEngine::createEndGamePacket() const {
        std::vector<uint8_t> packet(sizeof(network::PacketHeader));
        auto* header = reinterpret_cast<network::PacketHeader*>(packet.data());

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(network::PacketType::END_GAME_STATE);
        header->length = packet.size();
        header->sequence = 0;

        return packet;
    }

    void GameEngine::handleNetworkMessage(const std::vector<uint8_t>& data, [[maybe_unused]] const sockaddr_in& sender, const std::string& clientId) {
        if (data.size() < sizeof(network::PacketHeader)) return;

        const auto* header = reinterpret_cast<const network::PacketHeader*>(data.data());

        if (header->type == static_cast<uint8_t>(network::PacketType::PLAYER_INPUT)) {
            // Trouver l'entité correspondant au client
            auto it = playerEntities.find(clientId);
            if (it == playerEntities.end()) return;

            EntityID playerEntity = it->second;
            // Nous ne vérifions que cette entité spécifique
            if (entities.hasComponent<Position>(playerEntity) &&
                entities.hasComponent<Velocity>(playerEntity)) {
                auto& vel = entities.getComponent<Velocity>(playerEntity);
                auto& input = entities.getComponent<InputComponent>(playerEntity);
                const auto* inputPacket = reinterpret_cast<const network::PlayerInputPacket*>(
                    data.data() + sizeof(network::PacketHeader));

                if (inputPacket->space && !entities.hasComponent<Projectile>(playerEntity)) {
                    input.space = true;
                }

                if (inputPacket->ultimate && !entities.hasComponent<Projectile>(playerEntity)) {
                    input.Ultimate = true;
                }

                if (input.space) {
                    shoot_system_.update(entities, playerEntity, false );
                    input.space = false;
                }

                if (input.Ultimate) {
                    shoot_system_.update(entities, playerEntity, true );
                    input.Ultimate = false;
                }

                if (!entities.hasComponent<Projectile>(playerEntity) && !inputPacket->space && !inputPacket->ultimate) {
                    vel.dx = 0.0f;
                    vel.dy = 0.0f;
                    if (inputPacket->left) vel.dx = -speed;
                    if (inputPacket->right) vel.dx = speed;
                    if (inputPacket->up) vel.dy = -speed;
                    if (inputPacket->down) vel.dy = speed;
                }
                }
        }
    }


    void GameEngine::handleMessage(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender) {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(sender.port());
        addr.sin_addr.s_addr = sender.address().to_v4().to_ulong();

        // Créez un identifiant client à partir de l'IP et du port
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
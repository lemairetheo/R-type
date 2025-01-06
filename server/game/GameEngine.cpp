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
        for (size_t i = 0; i < NB_ENEMIES; i++) {
            float delay = static_cast<float>(i) * 2.0f;
            float x = static_cast<float>(800);
            float y = static_cast<float>(rand() % 600);
            enemySpawnQueue.push_back(PendingSpawn{delay, x, y});
        }
    }

    void GameEngine::broadcastWorldState() {
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
                if (entities.hasComponent<Projectile>(entity) && !entities.hasComponent<Enemy>(entity)) {
                    if (entities.getComponent<Projectile>(entity).isUltimate)
                        update->type = 5;
                    else
                        update->type = 1;
                } else if (entities.hasComponent<Enemy>(entity)) {
                    auto it = entities.hasTypeEnemy<Enemy>(entity);
                    update->type = it;
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
        entities.addComponent(playerEntity, Player{0, 3});
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

        handleEnemySpawns(dt);
        handleEnemyShoot();
        handleCollisions();

        for (auto& system : systems) {
            system->update(entities, dt);
        }

        broadcastWorldState();
    }

    void GameEngine::handleEnemySpawns(float dt) {
        for (auto it = enemySpawnQueue.begin(); it != enemySpawnQueue.end(); ) {
            it->delay -= dt;
            if (it->delay <= 0) {
                spawnEnemy(it->x, it->y, 2);
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
        auto players = entities.getEntitiesWithComponents<Player>();

        for (EntityID missile : missiles) {
            if (!entities.hasComponent<Position>(missile)) continue;

            const auto& missilePos = entities.getComponent<Position>(missile);
            const float missileRadius = 5.0f;
            bool isUltimate = entities.getComponent<Projectile>(missile).isUltimate;

            // Handle collision with enemies
            for (EntityID enemy: enemies) {
                if (!entities.hasComponent<Position>(enemy)) continue;

                const auto& enemyPos = entities.getComponent<Position>(enemy);
                const float enemyRadius = 20.0f;

                if (checkCollision(missilePos, missileRadius, enemyPos, enemyRadius) && entities.getComponent<Projectile>(missile).lunchByType != 2) {
                    handleCollision(missile, enemy);
                    if (!isUltimate) {
                        break;
                    }
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
        }
    }

    void GameEngine::handleCollision(EntityID missile, EntityID enemy) {
        auto& projectile = entities.getComponent<Projectile>(missile);
        entities.getComponent<Enemy>(enemy).life -= projectile.damage;

        if (entities.getComponent<Enemy>(enemy).life <= 0) {
            updatePlayerScore();
            auto packet = createEntityDeathPacket(missile, enemy);
            network.broadcast(packet);
            entities.destroyEntity(enemy);

            if (!projectile.isUltimate) {
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
            packet = createEntityDeathPacket(player, -1);
            network.broadcast(packet);
            exit(0); // Le joueur n'a plus de vie et est mort -> À modifier (le if) pour avoir le comportement souhaité
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

    bool GameEngine::checkCollision(const Position& pos1, float radius1, const Position& pos2, float radius2) {
        float dx = pos1.x - pos2.x;
        float dy = pos1.y - pos2.y;
        float distanceSquared = dx * dx + dy * dy;
        float radiusSum = radius1 + radius2;
        return distanceSquared <= (radiusSum * radiusSum);
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

            if (player.score >= 10) {
                broadcastEndGameState();
                break;
            }
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

    void GameEngine::handleNetworkMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender, const std::string& clientId) {
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
                    std::cout << "create ult" << std::endl;
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
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
                if (entities.hasComponent<Projectile>(entity) && !entities.hasComponent<Enemy>(entity))
                    update->type = 1;
                else if (entities.hasComponent<Enemy>(entity))
                    update->type = 2;
                else
                    update->type = 0;
                network.broadcast(packet);
            }
        }
    }

    EntityID GameEngine::createNewPlayer(const sockaddr_in& client) {
        std::string clientId = std::string(inet_ntoa(client.sin_addr)) + ":" + std::to_string(ntohs(client.sin_port));
        EntityID playerEntity = entities.createEntity();
        entities.addComponent(playerEntity, Position{400.0f, 300.0f});
        entities.addComponent(playerEntity, Velocity{0.0f, 0.0f});
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

        for (auto it = enemySpawnQueue.begin(); it != enemySpawnQueue.end(); ) {
            it->delay -= dt;
            if (it->delay <= 0) {
                EntityID enemyEntity = entities.createEntity();
                entities.addComponent(enemyEntity, Position{it->x, it->y});
                entities.addComponent(enemyEntity, Velocity{-50.0f, 0.0f});
                entities.addComponent(enemyEntity, Enemy{1, 1});

                it = enemySpawnQueue.erase(it);
            } else {
                ++it;
            }
        }

        for (EntityID missile : entities.getEntitiesWithComponents<Projectile>()) {
            if (!entities.hasComponent<Position>(missile)) continue;

            const auto& missilePos = entities.getComponent<Position>(missile);

            for (EntityID enemy : entities.getEntitiesWithComponents<Enemy>()) {
                if (!entities.hasComponent<Position>(enemy)) continue;

                const auto& enemyPos = entities.getComponent<Position>(enemy);
                float missileRadius = 5.0f;
                float enemyRadius = 20.0f;

                if (checkCollision(missilePos, missileRadius, enemyPos, enemyRadius)) {

                    entities.resetEntityComponents(enemy);
                    entities.destroyEntity(enemy);
                    entities.resetEntityComponents(missile);
                    entities.destroyEntity(missile);
                    std::cout << "Collision: Missile touche un ennemi !" << std::endl;
                    break; // Passe au prochain missile
                }
            }
        }

        for (auto& system : systems) {
            system->update(entities, dt);
        }

        broadcastWorldState();
    }

    void GameEngine::handleNetworkMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender) {
        std::string clientId = std::string(inet_ntoa(sender.sin_addr)) + ":" + std::to_string(ntohs(sender.sin_port));

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

                if (input.space) {
                    shoot_system_.update(entities, playerEntity);
                    input.space = false;
                }

                    if (!entities.hasComponent<Projectile>(playerEntity) && !inputPacket->space) {
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
    void GameEngine::handleMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender) {
        handleNetworkMessage(data, sender);
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
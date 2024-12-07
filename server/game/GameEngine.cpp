// server/game/GameEngine.cpp
#include "GameEngine.hpp"
#include <iostream>

namespace rtype::game {

    GameEngine::GameEngine(network::NetworkManager& networkManager)
        : network(networkManager) {
        EntityID playerEntity = entities.createEntity();
        entities.addComponent(playerEntity, Position{400.0f, 300.0f});
        entities.addComponent(playerEntity, Velocity{0.0f, 0.0f});
        systems.push_back(std::make_unique<MovementSystem>());
    }

    void GameEngine::broadcastWorldState() {
        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (entities.hasComponent<Position>(entity) && entities.hasComponent<Velocity>(entity)) {
                const auto& pos = entities.getComponent<Position>(entity);
                const auto& vel = entities.getComponent<Velocity>(entity);

                // Créer le paquet
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
                network.broadcast(packet);
            }
        }
    }


    void GameEngine::update() {
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastUpdate).count();
        lastUpdate = currentTime;

        for (auto& system : systems) {
            system->update(entities, dt);
        }

        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (entities.hasComponent<Position>(entity)) {
                const auto& pos = entities.getComponent<Position>(entity);
            }
        }
        broadcastWorldState();
    }

    void GameEngine::handleNetworkMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender) {
        if (data.size() < sizeof(network::PacketHeader)) return;

        const auto* header = reinterpret_cast<const network::PacketHeader*>(data.data());

        if (header->type == static_cast<uint8_t>(network::PacketType::PLAYER_INPUT)) {
            for (EntityID entity = 0; entity < MAX_ENTITIES; entity++) {
                if (entities.hasComponent<Position>(entity) && entities.hasComponent<Velocity>(entity)) {
                    auto& vel = entities.getComponent<Velocity>(entity);
                    const auto* inputPacket = reinterpret_cast<const network::PlayerInputPacket*>(data.data() + sizeof(network::PacketHeader));
                    vel.dx = 0.0f;
                    vel.dy = 0.0f;
                    if (inputPacket->left) vel.dx = -speed;
                    if (inputPacket->right) vel.dx = speed;
                    if (inputPacket->up) vel.dy = -speed;
                    if (inputPacket->down) vel.dy = speed;
                    break;
                }
            }
        }
    }

    void GameEngine::handleMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender) {
        handleNetworkMessage(data, sender);
    }

} // namespace rtype::game
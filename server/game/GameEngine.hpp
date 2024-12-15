#pragma once
#include "../shared/ecs/EntityManager.hpp"
#include "../shared/systems/System.hpp"
#include "../shared/abstracts/AEngine.hpp"
#include "../shared/systems/ShootSystem.hpp"
#include "../shared/network/packetType.hpp"
#include "../network/NetworkManager.hpp"
#include "../shared/systems/MouvementSystem.hpp"

namespace rtype::game {
    class GameEngine : public engine::AEngine {
    public:
        GameEngine(network::NetworkManager& networkManager);

        void broadcastWorldState();
        EntityID createNewPlayer(const sockaddr_in& sender);
        void update() override;
        void handleMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender) override;
    private:
        ShootSystem shoot_system_;
        EntityManager entities;
        void handleNetworkMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender);
        std::vector<std::unique_ptr<ISystem>> systems;
        network::NetworkManager& network;
        float speed = 300.0f;
        void handlePlayerDisconnection(const std::string& clientId);
        std::unordered_map<std::string, EntityID> playerEntities;
    };
}

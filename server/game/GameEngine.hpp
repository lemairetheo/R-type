#pragma once
#include "../shared/ecs/EntityManager.hpp"
#include "../shared/systems/System.hpp"
#include "../shared/abstracts/AEngine.hpp"
#include "../shared/network/packetType.hpp"
#include "../network/NetworkManager.hpp"
#include "MovementSystem.hpp"

namespace rtype::game {
    class GameEngine : public engine::AEngine {
    public:
        GameEngine(network::NetworkManager& networkManager);

        void broadcastWorldState();

        void update() override;
        void handleMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender) override;
    private:
        EntityManager entities;
        void handleNetworkMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender);
        std::vector<std::unique_ptr<ISystem>> systems;
        network::NetworkManager& network;
        float speed = 300.0f;
    };
}

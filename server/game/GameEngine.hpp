// server/GameEngine.hpp
#pragma once
#include "../shared/ecs/EntityManager.hpp"
#include "../shared/systems/System.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdint>
#include <iostream>


namespace rtype::game {

    class GameEngine {
    public:
        GameEngine();

        void update();
        void handleMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender);

    private:
        EntityManager entities;
        std::vector<std::unique_ptr<ISystem>> systems;
        std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
    };

} // namespace rtype::game
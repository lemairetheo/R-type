//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <iostream>
#include <vector>
#include <asio.hpp>
#include "network/NetworkManager.hpp"
#include "game/GameEngine.hpp"
#include <unordered_map>
#include "../shared/game/PlayerInfo.hpp"
#include "network/packetType.hpp"

namespace rtype {
    class Manager {
    public:
        Manager(uint16_t port);
        void start();
        void stop();
    private:
        void handleNewConnection(const asio::ip::udp::endpoint& sender);
        network::NetworkManager network;
        std::unique_ptr<game::GameEngine> game;
        std::unordered_map<std::string, PlayerInfo> players;
        std::atomic<bool> running;
        std::thread updateThread;
        void updateLoop();
    };
}



#endif //MANAGER_HPP

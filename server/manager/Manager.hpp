//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
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
        network::NetworkManager network;
        game::GameEngine _game;
        std::unordered_map<std::string, PlayerInfo> players;  // Pour gérer les clients connectés
        std::atomic<bool> running;
        std::thread updateThread;
        void updateLoop();
        void handleNewConnection(const sockaddr_in& sender);
    };
}



#endif //MANAGER_HPP

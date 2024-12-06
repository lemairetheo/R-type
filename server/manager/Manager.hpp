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


namespace rtype {
    // Manager.hpp
    class Manager {
    public:
        Manager(uint16_t port);
        void start();
        void stop();
    private:
        network::NetworkManager network;
        game::GameEngine game;  // Le constructeur devra prendre un NetworkManager
        std::atomic<bool> running;
        std::thread updateThread;
        void updateLoop();
    };
}



#endif //MANAGER_HPP

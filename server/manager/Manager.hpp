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
    class Manager {
        public:
            explicit Manager(uint16_t port) : network(port) {
                network.setMessageCallback([this](const std::vector<uint8_t>& data, const sockaddr_in& sender) {
                    std::cout << "Server received message from " << inet_ntoa(sender.sin_addr) << ":" << ntohs(sender.sin_port) << std::endl;
                    game.handleMessage(data, sender);
                    network.sendTo(res_buffer_test, sender);
                });
            }

            void start() {
                network.start();
            }

            void stop() {
                network.stop();
            }

        private:
            network::NetworkManager network;
            game::GameEngine game;
            std::vector<uint8_t> res_buffer_test = {0x00, 0x01, 0x02, 0x03};
    };
}



#endif //MANAGER_HPP

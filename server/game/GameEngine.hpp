#pragma once
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

namespace rtype::game {

class GameEngine {
public:
    GameEngine() {
        std::cout << "Game Engine initialized" << std::endl;
    }

    ~GameEngine() {
        std::cout << "Game Engine stopped" << std::endl;
    }

    void update() {
        // Dans le futur, on mettra ici la logique de mise à jour du jeu
        // Pour l'instant, on ne fait rien
    }

    void handleMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender) {
        std::cout << "////////// Game Engine received message from "
                  << inet_ntoa(sender.sin_addr) << ":" << ntohs(sender.sin_port)
                  << " with " << data.size() << " bytes" << std::endl;
        std::cout << "Message content: ";
        for (const auto& byte : data) {
            std::cout << static_cast<char>(byte);
        }
        std::cout << std::endl;
    }
};

} // namespace rtype::game
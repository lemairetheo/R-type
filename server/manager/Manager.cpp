#include "Manager.hpp"

namespace rtype {
    Manager::Manager(uint16_t port) : network(port), game(std::make_unique<game::GameEngine>(network)), running(false) {
        network.setMessageCallback([this](const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender) {
            const auto* header = reinterpret_cast<const network::PacketHeader*>(data.data());
            if (header->type == static_cast<uint8_t>(network::PacketType::CONNECT_REQUEST)) {
                game->handleMessage(data, sender);
                std::vector<uint8_t> response(sizeof(network::PacketHeader) + sizeof(network::ConnectResponsePacket));
                auto* respHeader = reinterpret_cast<network::PacketHeader*>(response.data());
                auto* connectResponse = reinterpret_cast<network::ConnectResponsePacket*>(
                    response.data() + sizeof(network::PacketHeader));

                EntityID playerId = game->createNewPlayer(sender);

                respHeader->magic[0] = 'R';
                respHeader->magic[1] = 'T';
                respHeader->version = 1;
                respHeader->type = static_cast<uint8_t>(network::PacketType::CONNECT_RESPONSE);
                respHeader->length = response.size();
                respHeader->sequence = 0;

                connectResponse->success = true;
                connectResponse->playerId = playerId;

                network.sendTo(response, sender);
            } else {
                // Pour tous les autres messages, les transmettre directement au game
                game->handleMessage(data, sender);
            }
        });
    }

    void Manager::updateLoop() {
        while (running) {
            network.update();
            game->update();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void Manager::handleNewConnection(const asio::ip::udp::endpoint& sender) {
        std::vector<uint8_t> response(sizeof(network::PacketHeader) + sizeof(network::ConnectResponsePacket));
        auto* header = reinterpret_cast<network::PacketHeader*>(response.data());
        auto* connectResponse = reinterpret_cast<network::ConnectResponsePacket*>(
            response.data() + sizeof(network::PacketHeader));
        EntityID playerId = game->createNewPlayer(sender);
        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(network::PacketType::CONNECT_RESPONSE);
        header->length = response.size();
        header->sequence = 0;

        connectResponse->success = true;
        connectResponse->playerId = playerId;

        network.sendTo(response, sender);
    }

    void Manager::start() {
        running = true;
        network.start();
        updateThread = std::thread(&Manager::updateLoop, this);
    }

    void Manager::stop() {
        running = false;
        if (updateThread.joinable()) {
            updateThread.join();
        }
        network.stop();
    }
}
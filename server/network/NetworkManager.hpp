#pragma once
#include "../../shared/abstracts/ANetwork.hpp"
#include <asio.hpp>
#include <thread>
#include <vector>
#include <functional>
#include <unordered_map>
#include "network/packetType.hpp"

namespace rtype::network {
    class NetworkManager : public ANetwork {
    public:
        explicit NetworkManager(uint16_t port);
        ~NetworkManager() override;
        void update();
        void start() override;
        void stop() override;
        void setMessageCallback(std::function<void(const std::vector<uint8_t>&, const asio::ip::udp::endpoint&)> callback) override;
        void broadcast(const std::vector<uint8_t>& data);
        void sendTo(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& client);

    private:
        void startReceive();
        void handleReceive(const asio::error_code& error, std::size_t bytes_transferred);

        asio::ip::udp::endpoint sender_endpoint;
        asio::io_context io_context;
        asio::ip::udp::socket socket;
        std::thread io_thread;
        std::atomic<bool> running;
        std::vector<uint8_t> receive_buffer;
        std::unordered_map<std::string, asio::ip::udp::endpoint> clients;
        std::function<void(const std::vector<uint8_t>&, const asio::ip::udp::endpoint&)> messageCallback;
        void checkTimeouts();
        void updateClientActivity(const std::string& client);
        void handleClientDisconnection(const std::string& clientId);
        std::unordered_map<std::string, std::chrono::steady_clock::time_point> clientLastSeen;
    };
}
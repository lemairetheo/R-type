/**
* \file NetworkManager.hpp
 * \brief Declaration of the network management for the client.
 */

#pragma once
#include "../shared/abstracts/ANetwork.hpp"
#include <asio.hpp>
#include <thread>
#include <atomic>

namespace rtype::network {
    /**
     * \class NetworkClient
     * \brief Class that manages the network for the client side
     */
    class NetworkClient : public ANetwork {
    public:
        explicit NetworkClient(const std::string& serverIP, uint16_t serverPort);
        void start() override;
        void stop() override;
        void sendTo(const std::vector<uint8_t>& data);
        void setMessageCallback(std::function<void(const std::vector<uint8_t>&, const asio::ip::udp::endpoint&)> callback) override;

    private:
        void receiveLoop();
        void startReceive();
        void handleReceive(const asio::error_code& error, std::size_t bytes_transferred);

        asio::io_context io_context;
        std::string serverIP;
        asio::ip::udp::socket socket;
        asio::ip::udp::endpoint server_endpoint;
        std::thread io_thread;
        std::atomic<bool> running;
        std::vector<uint8_t> receive_buffer;
        std::function<void(const std::vector<uint8_t>&, const asio::ip::udp::endpoint&)> messageCallback;
    };
}
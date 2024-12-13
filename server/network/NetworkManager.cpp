#include "NetworkManager.hpp"
#include <iostream>
#include <cstring>

namespace rtype::network {

    NetworkManager::NetworkManager(uint16_t port) : ANetwork(port), sock(-1), running(false) {}

    NetworkManager::~NetworkManager() {
        stop();
    }

    void NetworkManager::start() {
        if (running)
            return;

        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            throw std::runtime_error("Failed to create socket");
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            close(sock);
            throw std::runtime_error("Failed to bind socket");
        }

        running = true;
        receiveThread = std::thread(&NetworkManager::receiveLoop, this);
        std::cout << "Network Manager started on port " << port << std::endl;
    }

    void NetworkManager::stop() {
        if (!running)
            return;

        running = false;
        if (receiveThread.joinable()) {
            receiveThread.join();
        }

        if (sock >= 0) {
            close(sock);
            sock = -1;
        }

        std::cout << "Network Manager stopped" << std::endl;
    }

    void NetworkManager::setMessageCallback(std::function<void(const std::vector<uint8_t>&, const sockaddr_in&)> callback) {
        messageCallback = std::move(callback);
    }

    void NetworkManager::broadcast(const std::vector<uint8_t>& data) {
        for (const auto& [id, client] : clients)
            this->sendTo(data, client);
    }

    void NetworkManager::sendTo(const std::vector<uint8_t>& data, const sockaddr_in& client) {
        if (sock < 0) return;
        sendto(sock, data.data(), data.size(), 0, (struct sockaddr*)&client, sizeof(client));
    }

    void NetworkManager::receiveLoop() {
        std::vector<uint8_t> buffer(1024);
        sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);

        while (running) {
            std::memset(&clientAddr, 0, sizeof(clientAddr));
            ssize_t received = recvfrom(sock, buffer.data(), buffer.size(), 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
            if (received > 0) {
                std::string clientId = std::string(inet_ntoa(clientAddr.sin_addr)) + ":" + std::to_string(ntohs(clientAddr.sin_port));
                clients[clientId] = clientAddr;
                if (messageCallback) {
                    messageCallback(std::vector(buffer.begin(), buffer.begin() + received), clientAddr);
                }
            }
        }
    }

} // namespace rtype::network
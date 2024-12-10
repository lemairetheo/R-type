//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

/**
 * \file NetworkManager.cpp
 * \brief Implementation of the network management for the client.
 */

#include "NetworkManager.hpp"

namespace rtype::network {

    void NetworkClient::start() {
        if (running) return;
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            throw std::runtime_error("Failed to create socket");
        }
        sockaddr_in clientAddr{};
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port = 0;
        clientAddr.sin_addr.s_addr = INADDR_ANY;
        if (bind(sock, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) < 0) {
            std::cout << "Client: Failed to bind: " << strerror(errno) << std::endl;
            close(sock);
            throw std::runtime_error("Failed to bind socket");
        }
        socklen_t len = sizeof(clientAddr);
        if (getsockname(sock, (struct sockaddr *)&clientAddr, &len) < 0) {
            std::cout << "Client: Failed to get socket name: " << strerror(errno) << std::endl;
        }
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // localhost
        running = true;
        receiveThread = std::thread(&NetworkClient::receiveLoop, this);
        std::cout << "Client: Network initialized and running" << std::endl;
    }

    void NetworkClient::stop() {
        if (!running) return;
        running = false;
        if (receiveThread.joinable())
            receiveThread.join();
        close(sock);
    }

    void NetworkClient::sendTo(const std::vector<uint8_t>& data) {
        sendto(sock, data.data(), data.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    }

    void NetworkClient::setMessageCallback(std::function<void(const std::vector<uint8_t>&, const sockaddr_in&)> callback) {
        messageCallback = std::move(callback);
    }

    void NetworkClient::receiveLoop() {
        std::cout << "Client: Receive loop started" << std::endl;
        std::vector<uint8_t> buffer(1024);
        sockaddr_in senderAddr;
        socklen_t senderLen = sizeof(senderAddr);

        while (running) {
            ssize_t received = recvfrom(sock, buffer.data(), buffer.size(), 0, (struct sockaddr*)&senderAddr, &senderLen);
            if (received < 0)
                std::cout << "Client: Error receiving: " << strerror(errno) << std::endl;
            else if (received > 0) {
                std::cout << "Client: Received " << received << " bytes" << std::endl;
                std::cout << "Client: Message: " << std::string(buffer.begin(), buffer.begin() + received) << std::endl;
                if (messageCallback)
                    messageCallback(std::vector<uint8_t>(buffer.begin(),buffer.begin() + received), senderAddr);
            }
        }
        std::cout << "Client: Receive loop ended" << std::endl;
    }
}


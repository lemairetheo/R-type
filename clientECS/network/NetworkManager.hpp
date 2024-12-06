#pragma once
#include "../shared/abstracts/ANetwork.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include <atomic>

namespace rtype::network {
    class NetworkClient : public ANetwork {
    public:
        explicit NetworkClient(uint16_t port) : ANetwork(port), running(false), sock(-1) {}

        void start() override;

        void stop() override;

        void sendTo(const std::vector<uint8_t>& data);

        void setMessageCallback(std::function<void(const std::vector<uint8_t>&, const sockaddr_in&)> callback) override;

    private:
        void receiveLoop();

        int sock;
        sockaddr_in serverAddr;
        std::thread receiveThread;
        std::atomic<bool> running;
        std::function<void(const std::vector<uint8_t>&, const sockaddr_in&)> messageCallback;
    };
}
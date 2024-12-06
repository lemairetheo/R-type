#pragma once
#include "../../shared/abstracts/ANetwork.hpp"
#include <thread>
#include <vector>
#include <functional>
#include <unordered_map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace rtype::network {

    class NetworkManager : public ANetwork {
    public:
        explicit NetworkManager(uint16_t port);
        ~NetworkManager() override;

        void start() override;
        void stop() override;
        void setMessageCallback(std::function<void(const std::vector<uint8_t>&, const sockaddr_in&)> callback) override;
        void broadcast(const std::vector<uint8_t>& data);
        void sendTo(const std::vector<uint8_t>& data, const sockaddr_in& client);

    private:
        void receiveLoop();

        int sock;
        std::atomic<bool> running;
        std::thread receiveThread;
        std::unordered_map<std::string, sockaddr_in> clients;
        std::function<void(const std::vector<uint8_t>&, const sockaddr_in&)> messageCallback;
    };

} // namespace rtype::network
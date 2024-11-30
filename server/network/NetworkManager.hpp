#pragma once
#include <cstring>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <functional>
#include <unordered_map>
#ifdef _WIN32
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    using socket_t = SOCKET;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using socket_t = int;
#endif

namespace rtype::network {

    class NetworkManager {
    public:
        explicit NetworkManager(uint16_t port);
        ~NetworkManager();

        void start();
        void stop();

        void setMessageCallback(std::function<void(const std::vector<uint8_t>&, const sockaddr_in&)> callback) {
            messageCallback = callback;
        }

        void broadcast(const std::vector<uint8_t>& data);
        void sendTo(const std::vector<uint8_t>& data, const sockaddr_in& client);

    private:
        void receiveLoop();

        socket_t sock;
        uint16_t port;
        std::atomic<bool> running;
        std::thread receiveThread;
        std::unordered_map<std::string, sockaddr_in> clients;
        std::function<void(const std::vector<uint8_t>&, const sockaddr_in&)> messageCallback;
    };

} // namespace rtype::network
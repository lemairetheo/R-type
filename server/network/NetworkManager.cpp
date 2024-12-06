#include "NetworkManager.hpp"



namespace rtype::network {
    NetworkManager::NetworkManager(uint16_t port) : sock(0), port(port), running(false) {
        #ifdef _WIN32
                    WSADATA wsaData;
                    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                        throw std::runtime_error("WSAStartup failed");
                    }
        #endif
    }

    NetworkManager::~NetworkManager() {
        stop();
        #ifdef _WIN32
            WSACleanup();
        #endif
    }

    void NetworkManager::start() {
        if (running)
          return;
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock < 0) {
            throw std::runtime_error("Failed to create socket");
        }
        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        if (bind(sock, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
            throw std::runtime_error("Bind failed");
        }
        running = true;
        receiveThread = std::thread(&NetworkManager::receiveLoop, this);
        std::cout << "Network Manager started on port " << port << std::endl;
    }

    void NetworkManager::stop() {
        if (!running) return;
        running = false;

        if (receiveThread.joinable()) {
            receiveThread.join();
        }

        close(sock);
        std::cout << "Network Manager stopped" << std::endl;
    }

    void NetworkManager::sendTo(const std::vector<uint8_t>& data, const sockaddr_in& client) const {
        sendto(sock, reinterpret_cast<const char*>(data.data()), data.size(), 0,
               reinterpret_cast<const struct sockaddr*>(&client), sizeof(client));
    }

    void NetworkManager::receiveLoop() {
        std::vector<uint8_t> buffer(1024);
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        while (running) {
            std::memset(&clientAddr, 0, sizeof(clientAddr));

            ssize_t received = recvfrom(sock, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0,
                                      reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);

            if (received > 0) {
                std::string clientId = std::string(inet_ntoa(clientAddr.sin_addr)) + ":" +
                    std::to_string(ntohs(clientAddr.sin_port));
                clients[clientId] = clientAddr;
                if (messageCallback)
                    messageCallback(std::vector<uint8_t>(buffer.begin(), buffer.begin() + received), clientAddr);
            }
        }
    }

} // namespace rtype::network
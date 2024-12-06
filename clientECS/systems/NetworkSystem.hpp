#pragma once
#include <functional>
#include <thread>
#include <atomic>
#include "shared/network/packetType.hpp"

namespace rtype {

    class NetworkClient {
    public:
        explicit NetworkClient(uint16_t serverPort) : port(serverPort), running(false) {}

        void setMessageCallback(std::function<void(const std::vector<uint8_t>&)> callback) {
            messageCallback = std::move(callback);
        }

        void start() {
            // TODO: Implémenter la connexion UDP
        }

        void stop() {
            // TODO: Implémenter la déconnexion
        }

        void send(const std::vector<uint8_t>& data) {
            // TODO: Implémenter l'envoi de données
        }

    private:
        uint16_t port;
        std::atomic<bool> running;
        std::function<void(const std::vector<uint8_t>&)> messageCallback;
    };

} // namespace rtype
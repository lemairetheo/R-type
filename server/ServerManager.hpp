#include "network/NetworkManager.hpp"
#include "game/GameEngine.hpp"

namespace rtype {

    class ServerManager {
    public:
        explicit ServerManager(uint16_t port) : network(port) {
            network.setMessageCallback([this](const std::vector<uint8_t>& data, const sockaddr_in& sender) {
                game.handleMessage(data, sender);
            });
        }

        void start() {
            network.start();
        }

        void stop() {
            network.stop();
        }

    private:
        network::NetworkManager network;
        game::GameEngine game;
    };

} // namespace rtype
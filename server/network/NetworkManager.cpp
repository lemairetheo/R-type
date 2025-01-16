#include "NetworkManager.hpp"

namespace rtype::network {

    NetworkManager::NetworkManager(uint16_t port)
        : ANetwork(port)
        , socket(io_context)
        , running(false)
        , receive_buffer(1024) {
    }

    NetworkManager::~NetworkManager() {
        stop();
    }

    void NetworkManager::start() {
        if (running) return;
        try {
            socket.open(asio::ip::udp::v4());
            socket.bind(asio::ip::udp::endpoint(asio::ip::address_v4::any(), port));
            running = true;
            startReceive();
            io_thread = std::thread([this]() {
                try {
                    io_context.run();
                } catch (const std::exception& e) {
                    std::cout << "Network error: " << e.what() << std::endl;
                }
            });
            std::cout << "Network Manager started on port " << port << std::endl;
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to start network: " + std::string(e.what()));
        }
    }

    void NetworkManager::stop() {
        if (!running) return;

        running = false;
        io_context.stop();

        if (socket.is_open()) {
            socket.close();
        }

        if (io_thread.joinable()) {
            io_thread.join();
        }

        std::cout << "Network Manager stopped" << std::endl;
    }

    void NetworkManager::setMessageCallback(
        std::function<void(const std::vector<uint8_t>&, const asio::ip::udp::endpoint&)> callback) {
        messageCallback = std::move(callback);
    }

    void NetworkManager::broadcast(const std::vector<uint8_t>& data) {
        for (const auto& [id, client] : clients) {
            sendTo(data, client);
        }
    }

    void NetworkManager::sendTo(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& client) {
        socket.async_send_to(
            asio::buffer(data),
            client,
            [](const asio::error_code& error, std::size_t /*bytes_transferred*/) {
                if (error) {
                    std::cout << "Send error: " << error.message() << std::endl;
                }
            }
        );
    }

    void NetworkManager::startReceive() {
        socket.async_receive_from(
            asio::buffer(receive_buffer),
            sender_endpoint,
            [this](const asio::error_code& error, std::size_t bytes_transferred) {
                this->handleReceive(error, bytes_transferred);
            }
        );
    }

    void NetworkManager::handleReceive(const asio::error_code& error, std::size_t bytes_transferred) {
        if (!error && bytes_transferred > 0) {
            const auto* header = reinterpret_cast<const PacketHeader*>(receive_buffer.data());
            std::string clientId = sender_endpoint.address().to_string() + ":" +
                                 std::to_string(sender_endpoint.port());

            if (header->type == static_cast<uint8_t>(PacketType::CONNECT_REQUEST)) {
                if (clients.find(clientId) == clients.end()) {
                    clients[clientId] = sender_endpoint;
                    std::cout << "New client connected: " << clientId << std::endl;
                }
            }

            if (messageCallback) {
                std::vector<uint8_t> received_data(
                    receive_buffer.begin(),
                    receive_buffer.begin() + bytes_transferred
                );
                messageCallback(received_data, sender_endpoint);
            }

            if (running) {
                startReceive();
            }
        } else if (error != asio::error::operation_aborted && running) {
            std::cout << "Receive error: " << error.message() << std::endl;
            startReceive();
        }
    }

    void NetworkManager::update() {
        checkTimeouts();
    }

    void NetworkManager::updateClientActivity(const std::string& clientId) {
        clientLastSeen[clientId] = std::chrono::steady_clock::now();
    }

    void NetworkManager::checkTimeouts() {
        auto now = std::chrono::steady_clock::now();
        std::vector<std::string> disconnectedClients;

        for (const auto& [clientId, lastSeen] : clientLastSeen) {
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastSeen).count() > 5) {
                disconnectedClients.push_back(clientId);
            }
        }

        for (const auto& clientId : disconnectedClients) {
            handleClientDisconnection(clientId);
        }
    }

    void NetworkManager::handleClientDisconnection(const std::string& clientId) {
        if (auto it = clients.find(clientId); it != clients.end()) {
            clients.erase(it);
            clientLastSeen.erase(clientId);

            std::vector<uint8_t> packet(sizeof(PacketHeader));
            auto* header = reinterpret_cast<PacketHeader*>(packet.data());
            header->magic[0] = 'R';
            header->magic[1] = 'T';
            header->version = 1;
            header->type = static_cast<uint8_t>(PacketType::DISCONNECT);
            header->length = packet.size();

            broadcast(packet);
        }
    }
}
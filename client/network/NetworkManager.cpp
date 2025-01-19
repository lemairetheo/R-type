/**
 * \file NetworkManager.cpp
 * \brief Implementation of the network management for the client.
 */

#include "NetworkManager.hpp"

#include "network/packetType.hpp"

namespace rtype::network {

    NetworkClient::NetworkClient(const std::string& serverIP, uint16_t serverPort):
        ANetwork(serverPort),
        io_context(),
        serverIP(serverIP),
        socket(io_context),
        running(false),
        receive_buffer(1024)
    {}

    void NetworkClient::start() {
        if (running) return;

        try {
            socket.open(asio::ip::udp::v4());
            socket.bind(asio::ip::udp::endpoint(asio::ip::address_v4::any(), 0));
            server_endpoint = asio::ip::udp::endpoint(
                asio::ip::address::from_string(serverIP == "localhost" ? "127.0.0.1" : serverIP),
                port
            );
            running = true;
            startReceive();
            io_thread = std::thread([this]() {
                try {
                    io_context.run();
                } catch (const std::exception& e) {
                    std::cout << "Client: Network error: " << e.what() << std::endl;
                }
            });
            std::cout << "Client: Connected to " << serverIP << ":" << port << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Client: Failed to start network: " << e.what() << std::endl;
            throw;
        }
    }

    void NetworkClient::stop() {
        if (!running) return;
        running = false;

        io_context.stop();
        if (socket.is_open()) {
            socket.close();
        }

        if (io_thread.joinable()) {
            io_thread.join();
        }
    }

    void NetworkClient::sendTo(const std::vector<uint8_t>& data) {
        socket.async_send_to(
            asio::buffer(data),
            server_endpoint,
        [](const asio::error_code& error, [[maybe_unused]] std::size_t bytes_transferred) {
                if (error) {
                    std::cout << "Client: Send error: " << error.message() << std::endl;
                }
            }
        );
    }

    void NetworkClient::setMessageCallback(
        std::function<void(const std::vector<uint8_t>&, const asio::ip::udp::endpoint&)> callback) {
        messageCallback = std::move(callback);
    }

    void NetworkClient::startReceive() {
        asio::ip::udp::endpoint sender_endpoint;
        socket.async_receive_from(
            asio::buffer(receive_buffer),
            sender_endpoint,
            [this](const asio::error_code& error, std::size_t bytes_transferred) {
                this->handleReceive(error, bytes_transferred);
            }
        );
    }

    void NetworkClient::handleReceive(const asio::error_code& error, std::size_t bytes_transferred) {
        if (!error && bytes_transferred > 0) {
            if (messageCallback) {
                std::vector<uint8_t> received_data(
                    receive_buffer.begin(),
                    receive_buffer.begin() + bytes_transferred
                );
                asio::ip::udp::endpoint sender_endpoint;
                messageCallback(received_data, sender_endpoint);
            }

            if (running) {
                startReceive();  // Continue receiving
            }
        } else if (error != asio::error::operation_aborted) {
            std::cout << "Client: Receive error: " << error.message() << std::endl;
            if (running) {
                startReceive();  // Try to continue receiving despite error
            }
        }
    }

    std::vector<uint8_t> NetworkClient::createConnectRequest(const std::string& username) {
        std::vector<uint8_t> packet(sizeof(PacketHeader) + sizeof(ConnectRequestPacket));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());
        auto* request = reinterpret_cast<ConnectRequestPacket*>(packet.data() + sizeof(PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::CONNECT_REQUEST);
        header->length = packet.size();
        header->sequence = 0;

        std::strncpy(request->username, username.c_str(), sizeof(request->username) - 1);
        request->username[sizeof(request->username) - 1] = '\0';

        return packet;
    }

    std::vector<uint8_t> NetworkClient::createDisconnectRequest() {
        std::vector<uint8_t> packet(sizeof(PacketHeader));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::DISCONNECT);
        header->length = packet.size();
        header->sequence = 0;

        return packet;
    }

    std::vector<uint8_t> NetworkClient::createPlayerInputPacket(const InputComponent& input) {
        std::vector<uint8_t> packet(sizeof(PacketHeader) + sizeof(PlayerInputPacket));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());
        auto* inputPacket = reinterpret_cast<PlayerInputPacket*>(packet.data() + sizeof(PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::PLAYER_INPUT);
        header->length = packet.size();
        header->sequence = 0;

        inputPacket->up = input.up;
        inputPacket->down = input.down;
        inputPacket->left = input.left;
        inputPacket->right = input.right;
        inputPacket->space = input.space;
        inputPacket->ultimate = input.Ultimate;

        return packet;
    }
}

/**
 * \file NetworkManager.cpp
 * \brief Implementation of the network management for the client.
 */

#include "NetworkManager.hpp"

namespace rtype::network {

    NetworkClient::NetworkClient(uint16_t port)
        : ANetwork(port)
        , socket(io_context)
        , running(false)
        , receive_buffer(1024) {
    }

    void NetworkClient::start() {
        if (running) return;

        try {
            socket.open(asio::ip::udp::v4());
            socket.set_option(asio::socket_base::reuse_address(true));

            // Bind to any available port
            socket.bind(asio::ip::udp::endpoint(asio::ip::address_v4::any(), 0));

            // Set up server endpoint
            server_endpoint = asio::ip::udp::endpoint(
                asio::ip::address::from_string("127.0.0.1"),
                port
            );

            running = true;
            startReceive();

            // Start the io_context in a separate thread
            io_thread = std::thread([this]() {
                try {
                    io_context.run();
                } catch (const std::exception& e) {
                    std::cout << "Client: Network error: " << e.what() << std::endl;
                }
            });

            std::cout << "Client: Network initialized and running" << std::endl;
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
}
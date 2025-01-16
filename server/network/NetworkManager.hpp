#pragma once
#include "../../shared/abstracts/ANetwork.hpp"
#include <asio.hpp>
#include <thread>
#include <vector>
#include <functional>
#include <unordered_map>
#include "network/packetType.hpp"

namespace rtype::network {
    /**
     * @class NetworkManager
     * @brief Manages network communication for the server.
     */
    class NetworkManager : public ANetwork {
    public:
        /**
         * @brief Constructs a new NetworkManager object.
         * @param port The port number to bind the server.
         */
        explicit NetworkManager(uint16_t port);
        ~NetworkManager() override;

        /**
         * @brief Starts the network manager and begins handling connections.
         */
        void start() override;
        /**
         * @brief Updates the network manager.
         */
        void update();
        /**
         * @brief Stops the network manager and terminates all connections.
         */
        void stop() override;
        /**
         * @brief Sets the message callback function.
         * @param callback The function to call when a message is received.
         */
        void setMessageCallback(std::function<void(const std::vector<uint8_t>&, const asio::ip::udp::endpoint&)> callback) override;
        /**
         * @brief Broadcasts a message to all connected clients.
         * @param data The message to send.
         */
        void broadcast(const std::vector<uint8_t>& data);
        /**
         * @brief Sends a message to a specific client.
         * @param data The message to send.
         * @param client The endpoint of the client to send to.
         */
        void sendTo(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& client);
    private:
        /**
         * @brief Starts receiving messages from clients.
         */
        void startReceive();
        /**
         * @brief Handles a received message.
         * @param error The error code from the receive operation.
        */
        void handleReceive(const asio::error_code& error, std::size_t bytes_transferred);

        asio::ip::udp::endpoint sender_endpoint; ///< The endpoint of the sender of the current message.
        asio::io_context io_context; ///< The IO context for the network manager.
        asio::ip::udp::socket socket; ///< The socket for the network manager.
        std::thread io_thread; ///< The thread for running the IO context.
        std::atomic<bool> running; ///< Indicates whether the network manager is running.
        std::vector<uint8_t> receive_buffer; ///< The buffer for receiving messages.
        std::unordered_map<std::string, asio::ip::udp::endpoint> clients; ///< The connected clients.
        std::function<void(const std::vector<uint8_t>&, const asio::ip::udp::endpoint&)> messageCallback; ///< The message callback function.
        void checkTimeouts(); ///< Checks for clients that have timed out.
        void updateClientActivity(const std::string& client); ///< Updates the activity time for a client.
        void handleClientDisconnection(const std::string& clientId); ///< Handles a client disconnection.
        std::unordered_map<std::string, std::chrono::steady_clock::time_point> clientLastSeen; ///< The last time a client was active.
    };
}
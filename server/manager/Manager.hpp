//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#pragma once

#include <iostream>
#include <vector>
#include <asio.hpp>
#include "network/NetworkManager.hpp"
#include "game/GameEngine.hpp"
#include <unordered_map>
#include "../shared/game/PlayerInfo.hpp"
#include "network/packetType.hpp"

namespace rtype {

    /**
     * @class Manager
     * @brief Manages the server operations including network communication and game state.
     */
    class Manager {
    public:
        /**
         * @brief Constructs a new Manager object.
         * @param port The port number to bind the server.
         */
        Manager(uint16_t port);

        /**
         * @brief Starts the server and begins handling connections.
         */
        void start();

        /**
         * @brief Stops the server and terminates all connections.
         */
        void stop();

    private:
        /**
         * @brief Handles a new connection from a client.
         * @param sender The endpoint of the new client.
         */
        void handleNewConnection(const asio::ip::udp::endpoint& sender);

        network::NetworkManager network; ///< Manages network communication.
        std::unique_ptr<game::GameEngine> game; ///< Manages the game logic and state.
        std::unordered_map<std::string, PlayerInfo> players; ///< Stores information about connected players.
        std::atomic<bool> running; ///< Indicates whether the server is running.
        std::thread updateThread; ///< Thread for running the update loop.

        /**
         * @brief The main loop for updating the game state.
         */
        void updateLoop();
    };

}
#pragma once
#include <vector>
#include <chrono>
#include <cstdint>
#include <asio.hpp>
#include <iostream>

namespace rtype::engine {
    /**
     * @brief Abstract class for the engine
     *
     */
    class AEngine {
    public:
        virtual ~AEngine() = default;
        virtual void update() = 0;
        virtual void handleMessage(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& sender) = 0;
    protected:
        std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
    };
}
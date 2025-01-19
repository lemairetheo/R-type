#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <asio.hpp>
#include <iostream>

namespace rtype {
    /**
     * @brief Abstract class for the network
     *
     */
    class ANetwork {
    public:
        ANetwork(uint16_t port) : port(port) {}
        virtual ~ANetwork() = default;
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void setMessageCallback(std::function<void(const std::vector<uint8_t>&, const asio::ip::udp::endpoint&)> callback) = 0;
    protected:
        uint16_t port;
    };
}
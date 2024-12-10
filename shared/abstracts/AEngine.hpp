#pragma once
#include <vector>
#include <chrono>
#include <cstdint>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

namespace rtype::engine {
    class AEngine {
    public:
        virtual ~AEngine() = default;
        virtual void update() = 0;
        virtual void handleMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender) = 0;
    protected:
        std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
    };
}
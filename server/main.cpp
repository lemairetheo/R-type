#include "manager/Manager.hpp"
#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

std::atomic<bool> running(true);

void signalHandler(int signum) {
    running = false;
    std::cout << "Interrupt signal (" << signum << ") received.\n";
}

int main(int argc, char** argv) {
    try {
        uint16_t port = std::atoi(argv[1]);
        if (argc > 1) {
            port = static_cast<uint16_t>(std::stoi(argv[1]));
        }

        std::cout << "Starting R-Type server on port " << port << std::endl;

        rtype::Manager manager(port);
        manager.start();
        std::signal(SIGINT, signalHandler);
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "Shutting down server..." << std::endl;
        manager.stop();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
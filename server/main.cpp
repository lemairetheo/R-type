#include "ServerManager.hpp"
#include <iostream>
#include <csignal>
#include <atomic>

static std::atomic<bool> running(true);
static rtype::ServerManager* globalServer = nullptr;

void signalHandler(int signum) {
    std::cout << "\nSignal (" << signum << ") received. Cleaning up...\n";
    running = false;
    if (globalServer) {
        globalServer->stop();
    }
}

int main(int argc, char* argv[]) {
    try {
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        uint16_t port = 4242;
        if (argc > 1) {
            port = static_cast<uint16_t>(std::stoi(argv[1]));
        }

        std::cout << "Starting R-Type server on port " << port << std::endl;

        rtype::ServerManager server(port);
        globalServer = &server;
        server.start();

        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        server.stop();
        globalServer = nullptr;

        std::cout << "Server shutdown completed." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
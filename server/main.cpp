#include "manager/Manager.hpp"
#include <iostream>

int main(int argc, char** argv) {
    try {
        uint16_t port = 4242;
        if (argc > 1) {
            port = static_cast<uint16_t>(std::stoi(argv[1]));
        }

        std::cout << "Starting R-Type server on port " << port << std::endl;

        rtype::Manager server(port);
        server.start();

        while (true)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

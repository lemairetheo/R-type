// Manager.cpp
#include "Manager.hpp"

rtype::Manager::Manager(uint16_t port) : network(port), game(network), running(false) {  // Passez network au lieu de port
    network.setMessageCallback([this](const std::vector<uint8_t>& data, const sockaddr_in& sender) {
        game.handleMessage(data, sender);
    });
}

void rtype::Manager::updateLoop() {
    while (running) {
        game.update();  // Met à jour la logique du jeu et envoie les entités
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }
}

void rtype::Manager::start() {
    running = true;
    network.start();
    updateThread = std::thread(&Manager::updateLoop, this);  // Démarre la boucle de mise à jour
}

void rtype::Manager::stop() {
    running = false;
    if (updateThread.joinable()) {
        updateThread.join();
    }
    network.stop();
}
#include "GameEngine.hpp"


rtype::game::GameEngine::GameEngine() {
    systems.push_back(std::make_unique<MovementSystem>());

    EntityID entity = entities.createEntity();
    entities.addComponent(entity, Position{100.0f, 100.0f});
    entities.addComponent(entity, Velocity{50.0f, 0.0f});
    entities.addComponent(entity, NetworkComponent{1});
}

void rtype::game::GameEngine::handleMessage(const std::vector<uint8_t>& data, const sockaddr_in& sender) {
    std::cout << "////////// Game Engine received message from "
              << inet_ntoa(sender.sin_addr) << ":" << ntohs(sender.sin_port)
              << " with " << data.size() << " bytes" << std::endl;
    std::cout << "Message content: ";
    for (const auto& byte : data) {
        std::cout << static_cast<char>(byte);
    }
    std::cout << std::endl;
}

void rtype::game::GameEngine::update() {
    auto currentTime = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(currentTime - lastUpdate).count();
    lastUpdate = currentTime;
    for (auto& system : systems) {
        system->update(entities, dt);
    }
}
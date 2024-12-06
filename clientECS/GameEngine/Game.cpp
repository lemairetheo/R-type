//
// Created by Jean-Baptiste  Azan on 06/12/2024.
//

#include "Game.hpp"


//Public methods
rtype::Game::Game() : window(sf::VideoMode(800, 600), "R-Type"), network(4242) {
    systems.push_back(std::make_unique<MovementSystem>());
    systems.push_back(std::make_unique<RenderSystem>(window));

    // Configuration du callback réseau
    network.setMessageCallback([this](const std::vector<uint8_t>& data) {
      //
    });
}

void rtype::Game::run() {
    this->network.start();
    while (this->window.isOpen()) {
        handleEvents();
        update();
        render();
    }
    this->network.stop();
}

//Private methods

void rtype::Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void rtype::Game::update() {
    auto currentTime = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(currentTime - lastUpdate).count();
    lastUpdate = currentTime;

    for (auto& system : systems) {
        system->update(entities, dt);
    }
}

void rtype::Game::render() {
    window.clear();
    for (auto& system : systems) {
        system->update(entities, 0);
    }
    window.display();
}
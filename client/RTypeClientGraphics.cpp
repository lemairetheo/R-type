#include "RTypeClientGraphics.hpp"

RTypeClientGraphics::RTypeClientGraphics(unsigned int width, unsigned int height, const std::string& title)
    : window(sf::VideoMode(width, height), title) {
    std::cout << "Window initialized: " << title << " (" << width << "x" << height << ")" << std::endl;
}

void RTypeClientGraphics::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color::White);

        window.display();
    }
}

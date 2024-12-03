#include "RTypeClientGraphics.hpp"

RTypeClientGraphics::RTypeClientGraphics(unsigned int width, unsigned int height, const std::string& title)
    : window(sf::VideoMode(width, height), title) {
    _status = sf::Socket::Status::Disconnected;
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

sf::TcpSocket& RTypeClientGraphics::getSocket() {
    return _socket;
}

sf::Socket::Status& RTypeClientGraphics::getStatus() {
    return _status;
}

void RTypeClientGraphics::setStatus(const sf::Socket::Status status) {
    _status = status;
}



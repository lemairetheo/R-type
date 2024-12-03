#include "RTypeClientGraphics.hpp"

namespace rtype::client {
    RTypeClientGraphics::RTypeClientGraphics(unsigned int width, unsigned int height, const std::string& title)
        : window(sf::VideoMode(width, height), title), _menu(nullptr), _width(width), _height(height) {
        _status = sf::Socket::Status::Disconnected;
        _menu = new menu::Menu(width, height);
        std::cout << "Window initialized: " << title << " (" << width << "x" << height << ")" << std::endl;
    }

    RTypeClientGraphics::~RTypeClientGraphics() {
        delete _menu;
    }

    void RTypeClientGraphics::run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            window.clear(sf::Color::Black);
            _menu->render(window, event);

            window.display();
        }
    }

    sf::UdpSocket& RTypeClientGraphics::getSocket() {
        return _socket;
    }

    sf::Socket::Status& RTypeClientGraphics::getStatus() {
        return _status;
    }

    void RTypeClientGraphics::setStatus(const sf::Socket::Status status) {
        _status = status;
    }
}



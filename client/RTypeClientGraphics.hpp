#ifndef RTYPECLIENTGRAPHICS_HPP
    #define RTYPECLIENTGRAPHICS_HPP


#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <iostream>
#include <SFML/Network.hpp>
#include "../shared/network/packetType.hpp"
#include "menu/Menu.hpp"
namespace rtype::client {
    namespace menu {
        class Menu;
    }
    class RTypeClientGraphics {
    public:
        RTypeClientGraphics(unsigned int width, unsigned int height, const std::string& title);

        ~RTypeClientGraphics();

        void run();

        sf::UdpSocket& getSocket();

        sf::Socket::Status& getStatus();

        void setStatus(sf::Socket::Status status);

    private:
        sf::RenderWindow window;
        sf::UdpSocket _socket;
        sf::Socket::Status _status;

        menu::Menu *_menu;
        unsigned int _width;
        unsigned int _height;
    };
}

#endif //RTYPECLIENTGRAPHICS_HPP

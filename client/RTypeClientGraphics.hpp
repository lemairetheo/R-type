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

class RTypeClientGraphics {
public:
    RTypeClientGraphics(unsigned int width, unsigned int height, const std::string& title);

    void run();

    sf::UdpSocket& getSocket();

    sf::Socket::Status& getStatus();

    void setStatus(sf::Socket::Status status);

private:
    sf::RenderWindow window;
    sf::UdpSocket _socket;
    sf::Socket::Status _status;
};

#endif //RTYPECLIENTGRAPHICS_HPP

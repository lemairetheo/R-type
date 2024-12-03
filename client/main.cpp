#include "RTypeClientGraphics.hpp"

void sendConnectRequest(sf::UdpSocket& socket) {
    uint8_t packet = static_cast<uint8_t>(rtype::network::PacketType::CONNECT_REQUEST);

    sf::IpAddress serverAddress = "127.0.0.1";
    unsigned short port = 4242;

    if (socket.send(&packet, sizeof(packet), serverAddress, port) == sf::Socket::Done) {
        std::cout << "CONNECT_REQUEST sent to server." << std::endl;
    } else {
        std::cerr << "Failed to send CONNECT_REQUEST to server." << std::endl;
    }
}

void receiveConnectResponse(sf::UdpSocket& socket) {
    uint8_t response;
    std::size_t received;
    sf::IpAddress sender;
    unsigned short port;

    if (socket.receive(&response, sizeof(response), received, sender, port) == sf::Socket::Done) {
        if (response == static_cast<uint8_t>(rtype::network::PacketType::CONNECT_RESPONSE)) {
            std::cout << "CONNECT_RESPONSE received from server. Connection successful!" << std::endl;
        } else {
            std::cerr << "Unexpected response from server: " << static_cast<int>(response) << std::endl;
        }
    } else {
        std::cerr << "Failed to receive CONNECT_RESPONSE from server." << std::endl;
    }
}

int main(void)
{
    rtype::client::RTypeClientGraphics graphics(1920, 1080, "R-Type Client");

    if (graphics.getSocket().bind(0) != sf::Socket::Done) {
        std::cerr << "Failed to bind UDP socket" << std::endl;
        return 1;
    }
    sendConnectRequest(graphics.getSocket());
    // receiveConnectResponse(graphics.getSocket());

    graphics.run();
    return 0;
}

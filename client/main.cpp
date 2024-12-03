#include "RTypeClientGraphics.hpp"

void sendConnectRequest(sf::TcpSocket& socket) {
    uint8_t packet = static_cast<uint8_t>(rtype::network::PacketType::CONNECT_REQUEST);

    if (socket.send(&packet, sizeof(packet)) == sf::Socket::Done) {
        std::cout << "CONNECT_REQUEST sent to server." << std::endl;
    } else {
        std::cerr << "Failed to send CONNECT_REQUEST to server." << std::endl;
    }
}

void receiveConnectResponse(sf::TcpSocket& socket) {
    uint8_t response;
    std::size_t received;

    if (socket.receive(&response, sizeof(response), received) == sf::Socket::Done) {
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
    RTypeClientGraphics graphics(800, 600, "R-Type Client");
    sf::Socket::Status status = graphics.getSocket().connect("127.0.0.1", 4242);
    graphics.setStatus(status);
    if (graphics.getStatus() != sf::Socket::Done) {
        std::cerr << "Error connecting to server" << std::endl;
    }
    sendConnectRequest(graphics.getSocket());
    receiveConnectResponse(graphics.getSocket());
    graphics.run();
    return 0;
}
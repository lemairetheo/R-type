#include <iostream>
#include "GameEngine/Game.hpp"

int main() {
    try {
        rtype::Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
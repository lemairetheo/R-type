#include <iostream>
#include "game/Game.hpp"
#include "menu/Menu.hpp"
#include <SFML/Graphics.hpp>

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
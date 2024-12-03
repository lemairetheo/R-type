#include "Menu.hpp"

namespace rtype::client::menu {
    Menu::Menu(unsigned int width, unsigned int height) : button(nullptr), _width(width), _height(height) {
        button = new ui::Button({400, 300}, {200, 50}, "Click Me", sf::Color::Black, sf::Color::White, sf::Color::Yellow);

    };

    const ui::Button &Menu::getButton() {
        return *button;
    }



    void Menu::render(sf::RenderWindow &window, sf::Event &event) {
        button->handleEvent(event, window);
        button->render(window);
    }




}
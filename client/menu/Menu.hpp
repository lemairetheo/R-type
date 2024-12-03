#ifndef MENU_HPP
#define MENU_HPP

#include "../RTypeClientGraphics.hpp"
#include "../input/button/Button.hpp"

namespace rtype::client::menu {
    class Menu {
    public:
        Menu(unsigned int width, unsigned int height);

        const ui::Button &getButton();

        void render(sf::RenderWindow &window, sf::Event &event);

    private:
        ui::Button *button;
        unsigned int _width;
        unsigned int _height;



    };
}
#endif //MENU_HPP

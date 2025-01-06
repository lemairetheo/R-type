#ifndef MENU_HPP
#define MENU_HPP

#include "./button/Button.hpp"

namespace rtype {
    class Menu {
    public:
        Menu(unsigned int width, unsigned int height);

        const Button &getPlayButton();

        void render(sf::RenderWindow &window, sf::Event &event);

        bool getIsPlaying();

    private:
        bool isPlaying = false;
        bool isInSettings = false;
        bool inMenu = false;
        Button *play_button;
        Button *settings_button;
        unsigned int _width;
        unsigned int _height;



    };
}
#endif //MENU_HPP
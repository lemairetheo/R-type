#ifndef MENU_HPP
#define MENU_HPP

#include "./button/Button.hpp"
#include <iostream>

namespace rtype {
    class Menu {
    public:
        Menu(unsigned int width, unsigned int height);

        const Button &getPlayButton();

        void render(sf::RenderWindow &window, sf::Event &event);

        bool getIsPlaying();

        bool getRightMode();

        bool getColorblindMode();

    private:
        bool isPlaying = false;
        bool isInSettings = false;
        bool inMenu = false;
        bool isInHelp = false;
        bool right_mode = false;
        bool left_mode = false;
        bool colorblind_mode = false;
        Button *play_button;
        Button *settings_button;
        Button *exit_settings_button;
        Button *left_mode_button;
        Button *right_mode_button;
        Button *colorblind_mode_button;
        Button *help_button;
        Button *normal_mode_button;
        unsigned int _width;
        unsigned int _height;
        sf::Text help_text;
        sf::Text title_text;
        sf::Text settings_text;
        sf::Text help_description_text;
        sf::Font font;

    };
}
#endif //MENU_HPP
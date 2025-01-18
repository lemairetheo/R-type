#ifndef MENU_HPP
#define MENU_HPP

#include "./button/Button.hpp"
#include "./TextInput.hpp"
#include "utils/UiHelpers.hpp"
#include <iostream>

namespace rtype {
    class Menu {
    public:
        Menu(unsigned int width, unsigned int height);
        ~Menu();

        const Button &getPlayButton();
        void render(sf::RenderWindow &window, sf::Event &event);
        bool getIsPlaying() const;
        bool getRightMode() const;
        bool getColorblindMode() const;
        std::string getServerIP() const;
        uint16_t getServerPort() const;
        std::string getUsername() const;
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
        TextInput *ip_input;
        TextInput *port_input;
        unsigned int _width;
        unsigned int _height;
        sf::Text help_text;
        sf::Text title_text;
        sf::Text settings_text;
        sf::Text help_description_text;
        sf::Text ip_label;
        sf::Text port_label;
        sf::Font font;
        sf::Text username_label;
        TextInput *username_input;
    };
}
#endif //MENU_HPP
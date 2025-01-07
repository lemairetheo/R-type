#include "Menu.hpp"

#include <sys/syscall.h>

namespace rtype {
    Menu::Menu(unsigned int width, unsigned int height) : play_button(nullptr), _width(width), _height(height) {
        play_button = new Button({400, 500}, {200, 50}, "PLAY", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::White);
        settings_button = new Button({650, 500}, {200, 50}, "SETTINGS", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::White);
        inMenu = true;
        left_mode = true;
        exit_settings_button = new Button({400, 400}, {200, 50}, "EXIT", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::White);
        right_mode_button = new Button({550, 200}, {200, 50}, "RIGHT-HANDED MODE", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::Red);
        left_mode_button = new Button({250, 200}, {200, 50}, "LEFT-HANDED MODE", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::Red);
        colorblind_mode_button = new Button({400, 300}, {200, 50}, "COLORBLIND MODE", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::Red);
        help_button = new Button({150, 500}, {200, 50}, "HELP", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::Red);

        if (!font.loadFromFile("./client/assets/fonts/Roboto-Medium.ttf")) {
            std::cerr << "Erreur : Impossible de charger la police !\n";
            exit(84);
        }

        help_text.setFont(font);
        help_text.setString("HELP");
        help_text.setCharacterSize(24);
        help_text.setFillColor(sf::Color::White);
        help_text.setStyle(sf::Text::Bold | sf::Text::Italic);
        help_text.setPosition(400.f, 100.f);

        title_text.setFont(font);
        title_text.setString("R-Type");
        title_text.setCharacterSize(55);
        title_text.setFillColor(sf::Color::White);
        title_text.setStyle(sf::Text::Bold | sf::Text::Italic);
        title_text.setPosition(300.f, 100.f);
    };

    const Button &Menu::getPlayButton() {
        return *play_button;
    }

    void Menu::render(sf::RenderWindow &window, sf::Event &event) {
        if (inMenu) {
            if (play_button->handleEvent(event, window) == true) {
                isPlaying = true;
                inMenu = false;
            }
            play_button->render(window, "play");

            if (settings_button->handleEvent(event, window) == true) {
                isInSettings = true;
                inMenu = false;
            }
            settings_button->render(window, "settings");

            if (help_button->handleEvent(event, window) == true) {
                isInHelp = true;
                inMenu = false;
            }
            help_button->render(window, "help");
            std::cout << "avant le draw" << std::endl;
            window.draw(title_text);
            std::cout << "apres le draw" << std::endl;
        }
        if (isInSettings) {
            if (exit_settings_button->handleEvent(event, window) == true) {
                isInSettings = false;
                inMenu = true;
            }
            exit_settings_button->render(window, "exit");
            if (colorblind_mode_button->handleEvent(event, window) == true) {
                if (colorblind_mode == true) {
                    colorblind_mode = false;
                } else if ( colorblind_mode == false) {
                    colorblind_mode = true;
                }
            }
            if (colorblind_mode == true) {
                colorblind_mode_button->render(window, "activated");
            } else if ( colorblind_mode == false) {
                colorblind_mode_button->render(window, "desactivated");
            }

            if (left_mode_button->handleEvent(event, window) == true) {
                left_mode = true;
                right_mode = false;
                left_mode_button->setRectangleShape(sf::Color::Yellow);
                right_mode_button->setRectangleShape(sf::Color::White);
            }
            if (right_mode_button->handleEvent(event, window) == true) {
                left_mode = false;
                right_mode = true;
                left_mode_button->setRectangleShape(sf::Color::White);
                right_mode_button->setRectangleShape(sf::Color::Yellow);
            }
            if (right_mode) {
                right_mode_button->render(window, "right");
                left_mode_button->render(window, "right");
            } else if (left_mode) {
                right_mode_button->render(window, "left");
                left_mode_button->render(window, "left");
            }

        }
        if (isInHelp) {
            if (exit_settings_button->handleEvent(event, window) == true) {
                isInHelp = false;
                inMenu = true;
            }
            exit_settings_button->render(window, "EXIT");
            window.draw(help_text);
        }
    }

    bool Menu::getIsPlaying() {
        return isPlaying;
    }

    bool Menu::getRightMode() {
        return right_mode;
    }

    bool Menu::getColorblindMode() {
        return colorblind_mode;
    }




}

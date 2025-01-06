#include "Menu.hpp"

#include <sys/syscall.h>

namespace rtype {
    Menu::Menu(unsigned int width, unsigned int height) : play_button(nullptr), _width(width), _height(height) {
        play_button = new Button({550, 500}, {200, 50}, "PLAY", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::White);
        settings_button = new Button({200, 500}, {200, 50}, "SETTINGS", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::White);
        inMenu = true;
        left_mode = true;
        exit_settings_button = new Button({400, 500}, {200, 50}, "EXIT", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::White);
        right_mode_button = new Button({550, 200}, {200, 50}, "RIGHT-HANDED MODE", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::White);
        left_mode_button = new Button({250, 200}, {200, 50}, "LEFT-HANDED MODE", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::White);
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
            play_button->render(window);
            if (settings_button->handleEvent(event, window) == true) {
                isInSettings = true;
                inMenu = false;
            }
            settings_button->render(window);
        }
        if (isInSettings) {
            if (exit_settings_button->handleEvent(event, window) == true) {
                isInSettings = false;
                inMenu = true;
            }
            exit_settings_button->render(window);
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
            right_mode_button->render(window);
            left_mode_button->render(window);
        }
    }

    bool Menu::getIsPlaying() {
        return isPlaying;
    }



}

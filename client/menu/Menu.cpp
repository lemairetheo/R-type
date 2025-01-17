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
        colorblind_mode_button = new Button({250, 300}, {200, 50}, "COLORBLIND MODE", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::Red);
        help_button = new Button({150, 500}, {200, 50}, "HELP", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::Red);
        normal_mode_button = new Button({550, 300}, {200, 50}, "NORMAL MODE", sf::Color::Black, sf::Color::White, sf::Color::Yellow, sf::Color::Red);
        ip_input = new TextInput({300, 300}, {200, 40}, "Enter IP", font);
        port_input = new TextInput({300, 350}, {200, 40}, "Enter Port", font);
        username_input = new TextInput({300, 400}, {200, 40}, "Enter Username", font);
        if (!font.loadFromFile("assets/fonts/Roboto-Medium.ttf")) {
            std::cerr << "Erreur : Impossible de charger la police !\n";
            exit(84);
        }
        help_text = UiHelpers::createText("HELP", font, sf::Color::White, {350.f, 50.f}, 55, static_cast<sf::Text::Style>(sf::Text::Bold | sf::Text::Italic));
        ip_label = UiHelpers::createText("Server IP:", font, sf::Color::White, {190, 310}, 24, sf::Text::Bold);
        port_label = UiHelpers::createText("Port:", font, sf::Color::White, {190, 360}, 24, sf::Text::Bold);
        title_text = UiHelpers::createText("R-Type", font, sf::Color::White, {300.f, 100.f}, 55, static_cast<sf::Text::Style>(sf::Text::Bold | sf::Text::Italic));
        settings_text = UiHelpers::createText("SETTINGS", font, sf::Color::White, {270.f, 50.f}, 55, static_cast<sf::Text::Style>(sf::Text::Bold | sf::Text::Italic));
        help_description_text = UiHelpers::createText("The goal of this game is to defeat the final boss without dying.\nThere are 3 waves of enemies with 3 different monsters before \nreaching the final boss. Press space to fire missiles, Z to go up,\nQ to go left, S to go down and D to go right.", font, sf::Color::White, {100.f, 150.f}, 20, static_cast<sf::Text::Style>(sf::Text::Bold | sf::Text::Italic));
        username_label = UiHelpers::createText("Username:", font, sf::Color::White, {110, 410}, 24, sf::Text::Bold);
    };

    Menu::~Menu() {
        delete play_button;
        delete settings_button;
        delete exit_settings_button;
        delete left_mode_button;
        delete right_mode_button;
        delete colorblind_mode_button;
        delete help_button;
        delete normal_mode_button;
        delete ip_input;
        delete port_input;
        delete username_input;
    }

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
            username_input->handleEvent(event, window);
            username_input->render(window, "username");
            window.draw(title_text);
            window.draw(ip_label);
            window.draw(port_label);
            window.draw(username_label);
            ip_input->handleEvent(event, window);
            port_input->handleEvent(event, window);
            ip_input->render(window, "ip");
            port_input->render(window, "port");
            if (play_button->handleEvent(event, window) == true) {
                if (!ip_input->getValue().empty() && !port_input->getValue().empty() && !username_input->getValue().empty()) {
                    isPlaying = true;
                    inMenu = false;
                }
            }
        }
        if (isInSettings) {
            if (exit_settings_button->handleEvent(event, window) == true) {
                isInSettings = false;
                inMenu = true;
            }
            exit_settings_button->render(window, "exit");
            if (colorblind_mode_button->handleEvent(event, window) == true) {
                if ( colorblind_mode == false) {
                    colorblind_mode = true;
                }
            }
            if (normal_mode_button->handleEvent(event, window) == true) {
                if (colorblind_mode == true) {
                    colorblind_mode = false;
                }
            }
            if (colorblind_mode == true) {
                colorblind_mode_button->render(window, "activated");
                normal_mode_button->render(window, "desactivated");
            } else if ( colorblind_mode == false) {
                colorblind_mode_button->render(window, "desactivated");
                normal_mode_button->render(window, "activated");
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
            window.draw(settings_text);

        }
        if (isInHelp) {
            if (exit_settings_button->handleEvent(event, window) == true) {
                isInHelp = false;
                inMenu = true;
            }
            exit_settings_button->render(window, "EXIT");
            window.draw(help_text);
            if (left_mode)
                help_description_text.setString("The goal of this game is to defeat the final boss without dying.\nThere are 3 waves of enemies with 3 different monsters before \nreaching the final boss. Press space to fire missiles, X to throw\nan ultimate, Z to go up, Q to go left, S to go down and D to go right.");
            if (right_mode)
                help_description_text.setString("The goal of this game is to defeat the final boss without dying.\nThere are 3 waves of enemies with 3 different monsters before \nreaching the final boss. Press space to fire missiles, N to throw\nan ultimate, I to go up, J to go left, K to go down and L to go right.");

            window.draw(help_description_text);
        }
    }

    bool Menu::getIsPlaying() const {
        return isPlaying;
    }

    bool Menu::getRightMode() const {
        return right_mode;
    }

    bool Menu::getColorblindMode() const {
        return colorblind_mode;
    }

    std::string Menu::getServerIP() const {
        return ip_input->getValue();
    }

    std::string Menu::getUsername() const {
        return username_input->getValue();
    }

    uint16_t Menu::getServerPort() const {
        try {
            return static_cast<uint16_t>(std::stoi(port_input->getValue()));
        } catch (const std::exception& e) {
            return 0;
        }
    }

}

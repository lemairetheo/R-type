/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** TextInput
*/


#include "TextInput.hpp"

namespace rtype {
    TextInput::TextInput(const sf::Vector2f& position, const sf::Vector2f& size,
                        const std::string& placeholder, const sf::Font& font,
                        const sf::Color& bgColor, const sf::Color& txtColor)
        : isActive(false), backgroundColor(bgColor), textColor(txtColor),
          cursorBlinkTime(0), showCursor(false) {
        
        background.setPosition(position);
        background.setSize(size);
        background.setFillColor(backgroundColor);
        background.setOutlineThickness(2);
        background.setOutlineColor(sf::Color::Black);

        displayText.setFont(font);
        displayText.setCharacterSize(24);
        displayText.setFillColor(textColor);
        displayText.setPosition(position.x + 5, position.y + size.y/4);

        placeholderText.setFont(font);
        placeholderText.setString(placeholder);
        placeholderText.setCharacterSize(24);
        placeholderText.setFillColor(sf::Color(128, 128, 128));
        placeholderText.setPosition(position.x + 5, position.y + size.y/4);
    }

    bool TextInput::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        if (event.type == sf::Event::MouseButtonPressed) {
            if (background.getGlobalBounds().contains(worldPos)) {
                isActive = true;
                return true;
            } else {
                isActive = false;
            }
        }

        if (isActive) {
            if (event.type == sf::Event::TextEntered) {
                if (!isKeyPressed ||
                    keyRepeatClock.getElapsedTime().asSeconds() >= (isKeyPressed ? keyRepeatInterval : keyRepeatDelay)) {
                    if (event.text.unicode == '\b') {
                        if (!text.empty())
                            text.pop_back();
                    }
                    else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        text += static_cast<char>(event.text.unicode);
                    }
                    displayText.setString(text);
                    keyRepeatClock.restart();
                    isKeyPressed = true;
                }
            }
            else if (event.type == sf::Event::KeyReleased) {
                isKeyPressed = false;
                keyRepeatClock.restart();
            }
        }
        return false;
    }

    void TextInput::render(sf::RenderWindow& window, std::string type) {
        window.draw(background);
        
        if (text.empty()) {
            window.draw(placeholderText);
        } else {
            window.draw(displayText);
        }

        if (isActive) {
            cursorBlinkTime += 0.1f;
            if (cursorBlinkTime >= 0.5f) {
                showCursor = !showCursor;
                cursorBlinkTime = 0;
            }

            if (showCursor) {
                sf::RectangleShape cursor(sf::Vector2f(2, 30));
                cursor.setFillColor(textColor);
                cursor.setPosition(displayText.getPosition().x + displayText.getLocalBounds().width + 2,
                                 displayText.getPosition().y);
                window.draw(cursor);
            }
        }
    }

    void TextInput::setPosition(const sf::Vector2f& position) {
        background.setPosition(position);
        displayText.setPosition(position.x + 5, position.y + background.getSize().y/4);
        placeholderText.setPosition(position.x + 5, position.y + background.getSize().y/4);
    }

    void TextInput::setSize(const sf::Vector2f& size) {
        background.setSize(size);
    }

    void TextInput::setValue(const std::string& value) {
        text = value;
        displayText.setString(text);
    }
}
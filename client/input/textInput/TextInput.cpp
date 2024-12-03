#include "TextInput.hpp"

namespace ui {

TextInput::TextInput(const sf::Vector2f& position, const sf::Vector2f& size,
                     const sf::Color& textColor, const sf::Color& boxColor, const sf::Color& placeholderColor,
                     const std::string& placeholder)
    : isActive(false), onTextChanged(nullptr) {
    if (!font.loadFromFile("../client/asset/fonts/Arial.ttf")) {
        throw std::runtime_error("Failed to load font!");
    }

    // Setup box
    inputBox.setSize(size);
    inputBox.setPosition(position);
    inputBox.setFillColor(boxColor);
    inputBox.setOutlineColor(sf::Color::Black);
    inputBox.setOutlineThickness(1);

    // Setup input text
    inputText.setFont(font);
    inputText.setCharacterSize(16);
    inputText.setFillColor(textColor);
    updateTextPosition();

    // Setup placeholder text
    placeholderText.setFont(font);
    placeholderText.setCharacterSize(16);
    placeholderText.setFillColor(placeholderColor);
    placeholderText.setString(placeholder);
    placeholderText.setPosition(position.x + 5, position.y + 5);
}

void TextInput::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        // Activate input if clicked
        isActive = isMouseOver(window);
    }

    if (event.type == sf::Event::TextEntered && isActive) {
        if (event.text.unicode == 8 && !inputString.empty()) { // Backspace
            inputString.pop_back();
        } else if (event.text.unicode < 128 && event.text.unicode > 31) { // Printable characters
            inputString += static_cast<char>(event.text.unicode);
        }
        inputText.setString(inputString);
        updateTextPosition();

        // Call the onTextChanged callback if defined
        if (onTextChanged) {
            onTextChanged(inputString);
        }
    }
}

void TextInput::render(sf::RenderWindow& window) {
    window.draw(inputBox);

    // Render input text or placeholder
    if (inputString.empty() && !isActive) {
        window.draw(placeholderText);
    } else {
        window.draw(inputText);
    }
}

std::string TextInput::getValue() const {
    return inputString;
}

void TextInput::setPosition(const sf::Vector2f& position) {
    inputBox.setPosition(position);
    updateTextPosition();
}

void TextInput::setSize(const sf::Vector2f& size) {
    inputBox.setSize(size);
    updateTextPosition();
}

void TextInput::setBoxColor(const sf::Color& color) {
    inputBox.setFillColor(color);
}

void TextInput::setTextColor(const sf::Color& color) {
    inputText.setFillColor(color);
}

void TextInput::setOnTextChanged(std::function<void(const std::string&)> callback) {
    onTextChanged = callback;
}

void TextInput::updateTextPosition() {
    inputText.setPosition(inputBox.getPosition().x + 5, inputBox.getPosition().y + 5);
    placeholderText.setPosition(inputBox.getPosition().x + 5, inputBox.getPosition().y + 5);
}

bool TextInput::isMouseOver(const sf::RenderWindow& window) const {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    return inputBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

} // namespace ui

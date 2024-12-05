#include "Button.hpp"

namespace rtype::client::ui {

    Button::Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text,
                   const sf::Color& textColor, const sf::Color& normalColor, const sf::Color& hoverColor)
        : normalColor(normalColor), hoverColor(hoverColor), activeColor(sf::Color::Green),
          isActive(false), isHovered(false), currentScale(1.0f), targetScale(1.0f), onClick(nullptr) {
        if (!font.loadFromFile("../../client/asset/fonts/Arial.ttf")) {
            throw std::runtime_error("Failed to load font!");
        }

        buttonShape.setSize(size);
        buttonShape.setOrigin(size.x / 2.0f, size.y / 2.0f);
        buttonShape.setPosition(position);
        buttonShape.setFillColor(normalColor);

        buttonText.setFont(font);
        buttonText.setString(text);
        buttonText.setCharacterSize(16);
        buttonText.setFillColor(textColor);
        updateTextPosition();
    }

    void Button::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        if (isMouseOver(window)) {
            isHovered = true;
            targetScale = 1.1f;

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                isActive = !isActive;
                buttonShape.setFillColor(isActive ? activeColor : normalColor);

                if (onClick) {
                    onClick();
                }
            }
        } else {
            isHovered = false;
            targetScale = 1.0f;
        }
    }

    void Button::render(sf::RenderWindow& window) {
        currentScale += (targetScale - currentScale) * 0.1f;

        buttonShape.setScale(currentScale, currentScale);

        if (isHovered) {
            buttonShape.setFillColor(hoverColor);
        } else {
            buttonShape.setFillColor(isActive ? activeColor : normalColor);
        }

        buttonText.setCharacterSize(static_cast<unsigned int>(16 * currentScale));
        updateTextPosition();

        window.draw(buttonShape);
        window.draw(buttonText);
    }

    std::string Button::getValue() const {
        return isActive ? "ON" : "OFF";
    }

    void Button::setPosition(const sf::Vector2f& position) {
        buttonShape.setPosition(position);
        updateTextPosition();
    }

    void Button::setSize(const sf::Vector2f& size) {
        buttonShape.setSize(size);
        buttonShape.setOrigin(size.x / 2.0f, size.y / 2.0f);
    }

    void Button::setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& active) {
        normalColor = normal;
        hoverColor = hover;
        activeColor = active;
    }

    void Button::setText(const std::string& text) {
        buttonText.setString(text);
        updateTextPosition();
    }

    void Button::setTextColor(const sf::Color& color) {
        buttonText.setFillColor(color);
    }

    void Button::setOnClick(std::function<void()> callback) {
        onClick = callback;
    }

    void Button::updateTextPosition() {
        buttonText.setOrigin(buttonText.getLocalBounds().width / 2.0f, buttonText.getLocalBounds().height / 2.0f);
        buttonText.setPosition(buttonShape.getPosition());
    }

    bool Button::isMouseOver(const sf::RenderWindow& window) const {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return buttonShape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
    }

}
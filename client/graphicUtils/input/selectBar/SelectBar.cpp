#include "SelectBar.hpp"

namespace rtype::client::ui {

    SelectBar::SelectBar(const sf::Vector2f& position, const sf::Vector2f& size,
                         const sf::Color& barColor, const sf::Color& knobColor,
                         const sf::Color& hoverColor, const sf::Color& clickColor,
                         const sf::Color& containerColor)
        : currentValue(0), isDragging(false),
          normalKnobColor(knobColor), hoverKnobColor(hoverColor), clickKnobColor(clickColor) {
        if (!font.loadFromFile("../client/asset/fonts/Arial.ttf")) {
            throw std::runtime_error("Failed to load font!");
        }

        container.setSize({size.x + 60, size.y + 20});
        container.setPosition({position.x - 10, position.y - 10});
        container.setFillColor(containerColor);

        bar.setSize(size);
        bar.setPosition(position);
        bar.setFillColor(barColor);

        knob.setRadius(size.y / 2);
        knob.setFillColor(knobColor);
        knob.setOrigin(knob.getRadius(), knob.getRadius());
        updateKnobPosition();

        valueText.setFont(font);
        valueText.setCharacterSize(16);
        valueText.setFillColor(sf::Color::Black);
        updateValueText();
    }

    void SelectBar::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (isMouseOverKnob(window)) {
                isDragging = true;
                knob.setFillColor(clickKnobColor);
            }
        } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
            knob.setFillColor(isMouseOverKnob(window) ? hoverKnobColor : normalKnobColor);
        } else if (event.type == sf::Event::MouseMoved) {
            if (isDragging) {
                float mouseX = static_cast<float>(mousePos.x);
                float barStart = bar.getPosition().x;
                float barEnd = barStart + bar.getSize().x;

                if (mouseX < barStart) mouseX = barStart;
                if (mouseX > barEnd) mouseX = barEnd;

                float percentage = (mouseX - barStart) / bar.getSize().x;
                currentValue = static_cast<int>(percentage * 100);
                updateKnobPosition();
                updateValueText();
            } else {
                knob.setFillColor(isMouseOverKnob(window) ? hoverKnobColor : normalKnobColor);
            }
        }
    }

    void SelectBar::render(sf::RenderWindow& window) {
        window.draw(container);
        window.draw(bar);
        window.draw(knob);
        window.draw(valueText);
    }

    int SelectBar::getValue() const {
        return currentValue;
    }

    void SelectBar::setValue(int value) {
        if (value < 0) value = 0;
        if (value > 100) value = 100;
        currentValue = value;
        updateKnobPosition();
        updateValueText();
    }

    void SelectBar::setPosition(const sf::Vector2f& position) {
        container.setPosition({position.x - 10, position.y - 10});
        bar.setPosition(position);
        updateKnobPosition();
        updateValueText();
    }

    void SelectBar::setSize(const sf::Vector2f& size) {
        container.setSize({size.x + 60, size.y + 20});
        bar.setSize(size);
        knob.setRadius(size.y / 2);
        knob.setOrigin(knob.getRadius(), knob.getRadius());
        updateKnobPosition();
        updateValueText();
    }

    void SelectBar::setContainerColor(const sf::Color& color) {
        container.setFillColor(color);
    }

    void SelectBar::setContainerTexture(const sf::Texture& texture) {
        container.setTexture(&texture);
    }

    void SelectBar::updateKnobPosition() {
        float percentage = static_cast<float>(currentValue) / 100.f;
        float knobX = bar.getPosition().x + percentage * bar.getSize().x;
        float knobY = bar.getPosition().y + bar.getSize().y / 2.f;
        knob.setPosition(knobX, knobY);
    }

    void SelectBar::updateValueText() {
        valueText.setString(std::to_string(currentValue));
        float textX = bar.getPosition().x + bar.getSize().x + 15;
        float textY = bar.getPosition().y + (bar.getSize().y - valueText.getCharacterSize()) / 2;
        valueText.setPosition(textX, textY);
    }

    bool SelectBar::isMouseOverKnob(const sf::RenderWindow& window) const {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return knob.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
    }

} // namespace ui

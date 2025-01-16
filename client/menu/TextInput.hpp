#ifndef TEXT_INPUT_HPP
#define TEXT_INPUT_HPP

#include "Inputs.hpp"
#include <SFML/Graphics.hpp>

namespace rtype {
    class TextInput : public Inputs {
    public:
        TextInput(const sf::Vector2f& position, const sf::Vector2f& size, 
                 const std::string& placeholder, const sf::Font& font,
                 const sf::Color& bgColor = sf::Color::White,
                 const sf::Color& textColor = sf::Color::Black);

        bool handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
        void render(sf::RenderWindow& window, std::string type) override;
        std::string getValue() const override { return text; }
        void setPosition(const sf::Vector2f& position) override;
        void setSize(const sf::Vector2f& size) override;
        void setValue(const std::string& value);

    private:
        sf::RectangleShape background;
        sf::Text displayText;
        sf::Text placeholderText;
        std::string text;
        bool isActive;
        sf::Color backgroundColor;
        sf::Color textColor;
        float cursorBlinkTime;
        bool showCursor;
        sf::Clock keyRepeatClock;     // Pour gérer le temps entre les répétitions
        float keyRepeatDelay = 1.0f;  // Délai initial en secondes
        float keyRepeatInterval = 0.5f; // Intervalle entre les répétitions
        bool isKeyPressed = false;
    };
}

#endif // TEXT_INPUT_HPP
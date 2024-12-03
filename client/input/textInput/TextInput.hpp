#ifndef TEXTINPUT_HPP
#define TEXTINPUT_HPP

#include "../Inputs.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

namespace ui {

    class TextInput : public Inputs {
    public:
        TextInput(const sf::Vector2f& position, const sf::Vector2f& size,
                  const sf::Color& textColor = sf::Color::Black, const sf::Color& boxColor = sf::Color::White,
                  const sf::Color& placeholderColor = sf::Color(150, 150, 150),
                  const std::string& placeholder = "Enter text...");

        void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
        void render(sf::RenderWindow& window) override;

        std::string getValue() const override;
        void setPosition(const sf::Vector2f& position) override;
        void setSize(const sf::Vector2f& size) override;

        void setBoxColor(const sf::Color& color);
        void setTextColor(const sf::Color& color);

        void setOnTextChanged(std::function<void(const std::string&)> callback);

    private:
        sf::RectangleShape inputBox;
        sf::Text inputText;
        sf::Text placeholderText;
        sf::Font font;

        std::string inputString;
        bool isActive;

        std::function<void(const std::string&)> onTextChanged;

        void updateTextPosition();
        bool isMouseOver(const sf::RenderWindow& window) const;
    };

} // namespace ui

#endif // TEXTINPUT_HPP

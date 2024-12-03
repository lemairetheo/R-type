#ifndef SELECTBAR_HPP
#define SELECTBAR_HPP

    #include <SFML/Graphics.hpp>
    #include <string>

    namespace rtype::client::ui {

    class SelectBar {
        public:
            SelectBar(const sf::Vector2f& position, const sf::Vector2f& size,
                      const sf::Color& barColor = sf::Color::White, const sf::Color& knobColor = sf::Color::Green,
                      const sf::Color& hoverColor = sf::Color::Yellow, const sf::Color& clickColor = sf::Color::Red,
                      const sf::Color& containerColor = sf::Color(200, 200, 200));

            void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
            void render(sf::RenderWindow& window);

            int getValue() const;
            void setValue(int value);
            void setPosition(const sf::Vector2f& position);
            void setSize(const sf::Vector2f& size);
            void setContainerColor(const sf::Color& color);
            void setContainerTexture(const sf::Texture& texture);

        private:
            sf::RectangleShape container;
            sf::RectangleShape bar;
            sf::CircleShape knob;
            sf::Text valueText;
            sf::Font font;

            int currentValue;
            bool isDragging;

            sf::Color normalKnobColor;
            sf::Color hoverKnobColor;
            sf::Color clickKnobColor;

            void updateKnobPosition();
            void updateValueText();
            bool isMouseOverKnob(const sf::RenderWindow& window) const;
        };

    } // namespace ui

#endif // SELECTBAR_HPP

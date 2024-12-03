#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <SFML/Graphics.hpp>
#include <string>

namespace ui {

    class Inputs {
        public:
            virtual ~Inputs() = default;

            virtual void handleEvent(const sf::Event& event, const sf::RenderWindow& window) = 0;
            virtual void render(sf::RenderWindow& window) = 0;

            virtual std::string getValue() const = 0;
            virtual void setPosition(const sf::Vector2f& position) = 0;
            virtual void setSize(const sf::Vector2f& size) = 0;
    };

} // namespace ui

#endif // INPUTS_HPP

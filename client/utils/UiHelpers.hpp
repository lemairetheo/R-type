//
// Created by Jean-Baptiste  Azan on 16/01/2025.
//

#ifndef UIHELPERS_HPP
#define UIHELPERS_HPP

#include <SFML/Graphics.hpp>


namespace rtype {
    class UiHelpers {
    public:
        static sf::Text createText(const std::string &text, const sf::Font &font, const sf::Color &color, const sf::Vector2f &position, unsigned int size, sf::Text::Style style) {
            sf::Text txt;
            txt.setFont(font);
            txt.setString(text);
            txt.setCharacterSize(size);
            txt.setFillColor(color);
            txt.setStyle(style);
            txt.setPosition(position);
            return txt;
        }
    };
}



#endif //UIHELPERS_HPP

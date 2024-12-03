#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "../Inputs.hpp"
#include <SFML/Graphics.hpp>
#include <functional> // Pour std::function

namespace ui {

    class Button : public Inputs {
    public:
        Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text,
               const sf::Color& textColor = sf::Color::Black, const sf::Color& normalColor = sf::Color::White,
               const sf::Color& hoverColor = sf::Color::Yellow);

        // Implémentation des méthodes virtuelles
        void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
        void render(sf::RenderWindow& window) override;

        std::string getValue() const override; // Retourne "ON" ou "OFF"
        void setPosition(const sf::Vector2f& position) override;
        void setSize(const sf::Vector2f& size) override;

        // Personnalisation
        void setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& active);
        void setText(const std::string& text);
        void setTextColor(const sf::Color& color);
        void setOnClick(std::function<void()> callback); // Définir une fonction callback pour le clic

    private:
        sf::RectangleShape buttonShape;
        sf::Text buttonText;
        sf::Font font;

        sf::Color normalColor;
        sf::Color hoverColor;
        sf::Color activeColor;
        bool isActive;

        bool isHovered;
        float currentScale; // Échelle actuelle pour l'animation
        float targetScale;  // Échelle cible pour l'animation

        std::function<void()> onClick; // Callback pour le clic

        void updateTextPosition();
        bool isMouseOver(const sf::RenderWindow& window) const;
    };

} // namespace ui

#endif // BUTTON_HPP

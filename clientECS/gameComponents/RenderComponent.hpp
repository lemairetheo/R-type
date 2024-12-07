#pragma once
#include <SFML/Graphics.hpp>

struct RenderComponent {
    sf::Sprite sprite;
    float animationTimer = 0.0f;
    int currentFrame = 0;
    float frameTime = 0.1f;  // Temps par frame en secondes
    int frameCount = 5;      // Nombre de frames dans l'animation
    int frameWidth = 33;     // Largeur d'une frame
    int frameHeight = 17;    // Hauteur d'une frame
};
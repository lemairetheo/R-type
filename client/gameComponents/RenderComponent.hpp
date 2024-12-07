/**
 * @file RenderComponent.hpp
 * @brief Header file for RenderComponent
 * @see RenderComponent.cpp
 */

#pragma once
#include <SFML/Graphics.hpp>

/**
 * @struct RenderComponent
 * @brief Component for managing rendering and animation.
 */
struct RenderComponent {
    sf::Sprite sprite; ///< The sprite to render.
    float animationTimer = 0.0f; ///< The timer for the animation.
    int currentFrame = 0;
    float frameTime = 0.1f;
    int frameCount = 5;
    int frameWidth = 33;
    int frameHeight = 17;
};
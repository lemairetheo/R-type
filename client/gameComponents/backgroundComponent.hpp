/**
* \file backgroundComponent.hpp
 * \brief Defines the BackgroundComponent structure for the R-type project.
 */

#pragma once
#include <SFML/Graphics.hpp>

namespace rtype {
    /**
     * \struct BackgroundComponent
     * \brief Component for managing background rendering and scrolling.
     */
    struct BackgroundComponent {
        sf::Sprite sprite; ///< The sprite representing the background.
        float scrollSpeed = 50.0f; ///< The speed at which the background scrolls.
        float offsetX = 0.0f; ///< The current horizontal offset for scrolling.
        int layer = 0; ///< The layer of the background (0 = farthest, thus slowest).
    };
}
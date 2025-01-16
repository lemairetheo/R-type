/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** GameState
*/

#pragma once

namespace rtype {
    enum class GameState {
        MENU,           // Dans le menu principal
        CONNECTING,     // Tentative de connexion au serveur
        PLAYING,        // Jeu en cours
        GAME_OVER,     // Partie terminée (mort)
        VICTORY        // Partie gagnée
    };
}
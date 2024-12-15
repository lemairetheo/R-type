/*
** EPITECH PROJECT, 2024
** R_typed
** File description:
** PlayerInfo
*/
#include "../ecs/Entity.hpp"
#include <string>

struct PlayerInfo {
    EntityID entityId;
    std::string clientId;
    bool isConnected;
};
#pragma once
#include <cstdint>

namespace rtype {
    struct Position {
        float x;
        float y;
    };

    struct Velocity {
        float dx;
        float dy;
    };

    struct Player {
        int score;
        int life;
        int damage;
    };

    struct InputComponent {
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
        bool space = false;
        bool Ultimate = false;
    };

    struct PendingSpawn {
        float delay;
        float x;
        float y;
        int level;
    };

    struct NetworkComponent {
        uint32_t networkId;
    };

    struct Projectile {
        float damage;
        int lunchByType; // Si lancer par un joueur ou enemies (0 = player, 2 enemies)
        bool isActive;
        bool isUltimate;
    };

    struct Enemy {
        int damage;
        float life;
        int level;
        float speedShoot; // Velocity
    };

    struct HealthBonus {
        int healthAmount = 3;
    };

}
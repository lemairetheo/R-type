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
        int damage;
    };

    struct InputComponent {
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
        bool space = false;
    };

    struct PendingSpawn {
        float delay;
        float x;
        float y;
    };

    struct NetworkComponent {
        uint32_t networkId;
    };

    struct Projectile {
        float damage;
        bool isActive;
    };

    struct Enemy {
        int damage;
        float life;
    };
}
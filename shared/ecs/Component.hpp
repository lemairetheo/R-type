#pragma once
#include <cstdint>

/**
 * @brief Namespace for the ECS components
 */

namespace rtype {
    /**
     * @brief Position component for the ECS
     */
    struct Position {
        float x;
        float y;
    };
    /**
     * @brief Velocity component for the ECS
     */
    struct Velocity {
        float dx;
        float dy;
    };
    /**
     * @brief Player component for the ECS
     */
    struct Player {
        int score;
        int life;
        int damage;
    };
    /**
     * @brief InputComponent component for the ECS
     */
    struct InputComponent {
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
        bool space = false;
        bool Ultimate = false;
    };
    /**
     * @brief PendingSpawn component for the ECS
     */
    struct PendingSpawn {
        float delay;
        float x;
        float y;
        int level;
        bool isBoss;
    };
    /**
     * @brief NetworkComponent component for the ECS
     */
    struct NetworkComponent {
        uint32_t networkId;
    };
    /**
     * @brief Projectile component for the ECS
     */
    struct Projectile {
        float damage;
        int lunchByType;
        bool isActive;
        bool isUltimate;
    };
    /**
     * @brief Enemy component for the ECS
     */

    struct Enemy {
        int damage;
        float life;
        int level;
        float speedShoot;
        bool isBoss;
    };
    /**
     * @brief Health component for the ECS
     */
    struct Wall {
        int color;
    };
    /**
     * @brief Health component for the ECS
     */
    struct HealthBonus {
        int healthAmount = 3;
    };

}
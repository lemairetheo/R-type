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

    struct InputComponent {
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
    };

    struct NetworkComponent {
        uint32_t networkId;
    };
}
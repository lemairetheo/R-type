#include <cstdint>

struct Position {
    float x;
    float y;
};

struct Velocity {
    float dx;
    float dy;
};

struct NetworkComponent {
    uint32_t networkId;
};
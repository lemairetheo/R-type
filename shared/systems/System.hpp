#pragma once
#include "../ecs/EntityManager.hpp"

namespace rtype {
    class ISystem {
    public:
        virtual ~ISystem() = default;
        virtual void update(EntityManager& manager, float dt) = 0;
    };
}
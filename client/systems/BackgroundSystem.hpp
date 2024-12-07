//
// Created by Jean-Baptiste  Azan on 07/12/2024.
//

#ifndef BACKGROUNDSYSTEM_HPP
#define BACKGROUNDSYSTEM_HPP

#include <SFML/Graphics.hpp>
#include "ecs/EntityManager.hpp"
#include "shared/systems/System.hpp"
#include "../gameComponents/backgroundComponent.hpp"

namespace rtype {
    class BackgroundSystem : public ISystem {
    public:
        void update(EntityManager& manager, float dt) override;
    };
}


#endif //BACKGROUNDSYSTEM_HPP

//
// Created by Jean-Baptiste  Azan on 07/12/2024.
//

#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

#include <SFML/Graphics.hpp>
#include "shared/ecs/EntityManager.hpp"
#include "shared/systems/System.hpp"
#include "manager/ResourceManager.hpp"
#include "gameComponents/RenderComponent.hpp"


namespace rtype {
    class AnimationSystem : public ISystem {
    public:
        void update(EntityManager& manager, float dt) override;
    };
}

#endif //ANIMATIONSYSTEM_HPP

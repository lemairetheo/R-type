#pragma once

namespace rtype {
    class IComponent {
    public:
        virtual ~IComponent() = default;
        virtual void erase(EntityID entity) = 0;
    };
}
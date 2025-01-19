#pragma once

/**
 * @brief IComponent class
 * @details This class is used to define the interface of a component
 */
namespace rtype {
    class IComponent {
    public:
        virtual ~IComponent() = default;
        virtual void erase(EntityID entity) = 0;
    };
}
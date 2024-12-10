// shared/ecs/EntityManager.hpp
#pragma once
#include "Entity.hpp"
#include "SparseArray.hpp"
#include "Component.hpp"
#include "IComponent.hpp"
#include <unordered_map>
#include <typeindex>
#include <memory>

namespace rtype {
    constexpr size_t MAX_ENTITIES = 1000;

    class EntityManager {
    public:
        EntityManager();
        ~EntityManager() = default;

        EntityID createEntity();
        void destroyEntity(EntityID entity);

        template<typename Component>
        SparseArray<Component>& getComponents() {
            auto typeIndex = std::type_index(typeid(Component));
            auto it = _components.find(typeIndex);
            if (it == _components.end()) {
                auto sparseArray = std::make_unique<SparseArray<Component>>();
                auto ptr = sparseArray.get();
                _components[typeIndex] = std::move(sparseArray);
                return *ptr;
            }
            return *static_cast<SparseArray<Component>*>(_components[typeIndex].get());
        }

        template<typename Component>
        void addComponent(EntityID entity, Component component) {
            getComponents<Component>().insert_at(entity, component);
        }

        template<typename Component>
        Component& getComponent(EntityID entity) {
            return *getComponents<Component>()[entity];
        }

        template<typename Component>
        bool hasComponent(EntityID entity) const {
            auto typeIndex = std::type_index(typeid(Component));
            auto it = _components.find(typeIndex);
            if (it == _components.end())
                return false;
            return static_cast<const SparseArray<Component>*>(it->second.get())->operator[](entity).has_value();
        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponent>> _components;
        std::vector<EntityID> availableEntities;
        EntityID nextEntity = 0;
    };
}
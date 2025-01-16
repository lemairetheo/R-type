// shared/ecs/EntityManager.hpp
#pragma once
#include "Entity.hpp"
#include "SparseArray.hpp"
#include "Component.hpp"
#include "IComponent.hpp"
#include <unordered_map>
#include <typeindex>
#include <iostream>
#include <memory>

namespace rtype {
    constexpr size_t MAX_ENTITIES = 1000;
    constexpr size_t NB_ENEMIES = 50;

    class EntityManager {
    public:
        EntityManager();
        ~EntityManager() = default;

        EntityID createEntity();
        void reset();
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

        template<typename... Components>
        std::vector<EntityID> getEntitiesWithComponents() const {
            std::vector<EntityID> result;

            for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
                if ((hasComponent<Components>(entity) && ...)) {
                    result.push_back(entity);
                }
            }

            return result;
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

        template<typename Component>
        int hasTypeEnemy(EntityID entity) const {
            auto typeIndex = std::type_index(typeid(Component));
            auto it = _components.find(typeIndex);

            if (it == _components.end())
                return 0;

            const auto& sparseArray = *static_cast<const SparseArray<Component>*>(it->second.get());
            if (!sparseArray[entity].has_value())
                return 0;

            const auto& enemyComponent = sparseArray[entity].value();

            if (enemyComponent.level == 1)
                return 2;
            else if (enemyComponent.level == 2)
                return 3;
            else if (enemyComponent.level == 3)
                return 4;
            return 0;
        }

        void resetEntityComponents(EntityID entity) {
            for (auto& [typeIndex, componentArray] : _components) {
                if (componentArray) {
                    componentArray->erase(entity);
                }
            }
        }
    private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponent>> _components;
        std::vector<EntityID> availableEntities;
        EntityID nextEntity = 1;
        int NbEntities = 1;
    };
}
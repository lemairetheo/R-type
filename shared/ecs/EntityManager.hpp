#pragma once
#include "Entity.hpp"
#include "SparseArray.hpp"
#include "Component.hpp"
#include "IComponent.hpp"
#include <unordered_map>
#include <typeindex>
#include <iostream>

/**
 * @brief EntityManager class
 * @details This class is used to manage the entities and their components
 */
namespace rtype {
    /**
     * @brief Maximum number of entities that can exist simultaneously
     */
    constexpr size_t MAX_ENTITIES = 1000;
    /**
     * @brief Maximum number of enemies that can exist simultaneously
     */
    constexpr size_t NB_ENEMIES = 50;
    /**
     * @class EntityManager
     * @brief Manages entities and their components in an Entity Component System (ECS)
     *
     * @details The EntityManager is responsible for creating and destroying entities,
     * managing their components, and providing access to components through a sparse array system.
     */
    class EntityManager {
    public:
        EntityManager();
        ~EntityManager() = default;
        /**
         * @brief Creates a new entity
         * @return EntityID The ID of the newly created entity
         * @throw std::runtime_error if maximum number of entities is reached
         */
        EntityID createEntity();
        /**
         * @brief Resets the entire entity manager
         * @details Clears all components and resets entity counter to initial state
         */
        void reset();
        /**
         * @brief Destroys an entity and removes all its components
         * @param entity The ID of the entity to destroy
         */
        void destroyEntity(EntityID entity);
        /**
         * @brief Gets the sparse array of components of a specific type
         * @tparam Component The type of component to get
         * @return SparseArray<Component>& Reference to the sparse array of components
         */
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

        /**
         * @brief Gets all entities that have the specified components
         * @tparam Components Parameter pack of component types to check for
         * @return std::vector<EntityID> Vector of entity IDs that have all specified components
         */
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
        /**
         * @brief Adds a component to an entity
         * @tparam Component The type of component to add
         * @param entity The entity ID to add the component to
         * @param component The component instance to add
         */
        template<typename Component>
        void addComponent(EntityID entity, Component component) {
            getComponents<Component>().insert_at(entity, component);
        }
        /**
         * @brief Gets a component from an entity
         * @tparam Component The type of component to get
         * @param entity The entity ID to get the component from
         * @return Component& Reference to the requested component
         */
        template<typename Component>
        Component& getComponent(EntityID entity) {
            return *getComponents<Component>()[entity];
        }
        /**
         * @brief Checks if an entity has a specific component
         * @tparam Component The type of component to check for
         * @param entity The entity ID to check
         * @return bool True if the entity has the component, false otherwise
         */
        template<typename Component>
        bool hasComponent(EntityID entity) const {
            auto typeIndex = std::type_index(typeid(Component));
            auto it = _components.find(typeIndex);
            if (it == _components.end())
                return false;
            return static_cast<const SparseArray<Component>*>(it->second.get())->operator[](entity).has_value();
        }
        /**
         * @brief Checks the enemy type of an entity
         * @tparam Component The type of component to check (usually Enemy)
         * @param entity The entity ID to check
         * @return int Enemy type (2 for level 1, 3 for level 2, 4 for level 3, 0 if not an enemy)
         */
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
        /**
         * @brief Removes all components from an entity
         * @param entity The entity ID to reset
         */
        void resetEntityComponents(EntityID entity) {
            for (auto& [typeIndex, componentArray] : _components) {
                if (componentArray) {
                    componentArray->erase(entity);
                }
            }
        }
    private:
        /**
         * @brief Map storing component arrays indexed by their type
         */
        std::unordered_map<std::type_index, std::unique_ptr<IComponent>> _components;
        std::vector<EntityID> availableEntities;
        EntityID nextEntity = 1;
        int NbEntities = 1;
    };
}
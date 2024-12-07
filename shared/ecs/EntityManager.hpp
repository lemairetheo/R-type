#pragma once
#include "Entity.hpp"
#include "Component.hpp"
#include <vector>
#include <array>
#include <bitset>
#include <any>

namespace rtype {

const size_t MAX_ENTITIES = 1000;
const size_t MAX_COMPONENTS = 32;

class EntityManager {
    public:
        using ComponentMask = std::bitset<MAX_COMPONENTS>;
        EntityManager();
        EntityID createEntity();
        void destroyEntity(EntityID entity);
        template<typename T>


        void addComponent(EntityID entity, T component) {
            const size_t componentId = getComponentId<T>();
            ensureComponentVectorExists<T>();

            std::vector<T>& components = std::any_cast<std::vector<T>&>(componentArrays[componentId]);
            components[entity] = component;
            componentMasks[entity].set(componentId);
        }


        template<typename T>
        T& getComponent(EntityID entity) {
            const size_t componentId = getComponentId<T>();
            auto& components = std::any_cast<std::vector<T>&>(componentArrays[componentId]);
            return components[entity];
        }

        template<typename T>
        bool hasComponent(EntityID entity) const {
            return componentMasks[entity].test(getComponentId<T>());
        }

        ComponentMask getComponentMask(EntityID entity) const {
            return componentMasks[entity];
        }

    private:
        std::vector<EntityID> availableEntities;
        std::array<ComponentMask, MAX_ENTITIES> componentMasks;
        std::array<std::any, MAX_COMPONENTS> componentArrays;

        template<typename T>
        size_t getComponentId() const {
            static size_t id = nextComponentId++;
            return id;
        }

        template<typename T>
        void ensureComponentVectorExists() {
            size_t id = getComponentId<T>();
            if (!componentArrays[id].has_value()) {
                componentArrays[id] = std::vector<T>(MAX_ENTITIES);
            }
        }

        static size_t nextComponentId;
    };
}  // namespace rtype
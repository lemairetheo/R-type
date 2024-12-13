/*
** EPITECH PROJECT, 2024
** R_typed
** File description:
** EntityManager
*/
#include "EntityManager.hpp"

namespace rtype {
    EntityManager::EntityManager() {
        availableEntities.reserve(MAX_ENTITIES);
        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            availableEntities.push_back(entity);
        }
    }

    EntityID EntityManager::createEntity() {
        if (availableEntities.empty()) {
            throw std::runtime_error("Maximum number of entities reached");
        }
        EntityID id = availableEntities.back();
        availableEntities.pop_back();
        return id;
    }

    void EntityManager::destroyEntity(EntityID entity) {
        //resetEntityComponents(entity);
        availableEntities.push_back(entity);
    }
}
#include "EntityManager.hpp"

rtype::EntityManager::EntityManager() {
    // Initialiser le vecteur des entités disponibles
    for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
        availableEntities.push_back(entity);
    }
}

EntityID rtype::EntityManager::createEntity() {
    EntityID id = availableEntities.back();
    availableEntities.pop_back();
    return id;
}

void rtype::EntityManager::destroyEntity(EntityID entity) {
    componentMasks[entity].reset();
    availableEntities.push_back(entity);
}

namespace rtype {
    size_t EntityManager::nextComponentId = 0;
}
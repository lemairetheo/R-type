add_library(r-type-shared
        ecs/EntityManager.cpp
)

target_sources(r-type-shared
        PUBLIC
        ecs/Component.hpp
        ecs/Entity.hpp
        ecs/EntityManager.hpp
        ecs/IComponent.hpp
        ecs/SparseArray.hpp
        systems/System.hpp
        systems/MouvementSystem.hpp
        network/packetType.hpp
        abstracts/ANetwork.hpp
        abstracts/AEngine.hpp
)

target_include_directories(r-type-shared
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
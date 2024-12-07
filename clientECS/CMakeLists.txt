find_package(SFML 2.6.2 COMPONENTS graphics window system REQUIRED)

# Sources du client
set(CLIENT_SOURCES
        main.cpp
        network/NetworkManager.cpp
        systems/RenderSystem.cpp
        game/Game.cpp
        manager/ResourceManager.cpp
        manager/ResourceManager.hpp
        systems/AnimationSystem.cpp
        systems/AnimationSystem.hpp
)

add_executable(r-type_client ${CLIENT_SOURCES})

# Définir le chemin des assets
set(ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")

# Copier les assets
add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${ASSETS_DIR}"
)

# Ajouter la définition du chemin des assets
target_compile_definitions(r-type_client PRIVATE
        ASSETS_PATH="${ASSETS_DIR}"
)

target_include_directories(r-type_client
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
)

# Liens avec les bibliothèques
target_link_libraries(r-type_client
        PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        r-type-shared

)

# Options de compilation
if(UNIX)
    target_compile_options(r-type_client
            PRIVATE
            -Wall
            -Wextra
    )
endif()

# Configuration spécifique pour MSVC
if(MSVC)
    target_compile_options(r-type_client
            PRIVATE
            /W4
    )
endif()
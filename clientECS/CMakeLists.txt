find_package(SFML 2.6.2 COMPONENTS graphics window system REQUIRED)

# Sources du client
set(CLIENT_SOURCES
        main.cpp
        network/NetworkManager.cpp
        systems/RenderSystem.cpp
        game/Game.cpp
)

# Exécutable
add_executable(r-type_client ${CLIENT_SOURCES})

# Inclusions
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
# Configuration de SFML
set(SFML_STATIC_LIBRARIES TRUE)
set(BUILD_SHARED_LIBS FALSE)

# Recherche de SFML avec configuration spécifique
find_package(SFML COMPONENTS system window graphics audio CONFIG REQUIRED)

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
        systems/BackgroundSystem.cpp
        systems/BackgroundSystem.hpp
        menu/Menu.cpp
        menu/Menu.hpp
        menu/Inputs.hpp
        menu/button/Button.cpp
        menu/button/Button.hpp
)

add_executable(r-type_client WIN32 ${CLIENT_SOURCES})

# Configuration des assets
set(ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")

add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${ASSETS_DIR}"
)

target_compile_definitions(r-type_client PRIVATE
        ASSETS_PATH="${ASSETS_DIR}"
        SFML_STATIC
)

target_include_directories(r-type_client PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
)

# Liaison des bibliothèques
if(WIN32)
    target_link_libraries(r-type_client PRIVATE
            sfml-system
            sfml-window
            sfml-graphics
            sfml-audio
            sfml-main
            r-type-shared
            asio::asio
            opengl32
            winmm
            gdi32
            freetype
            ws2_32
            wsock32
    )
else()
    target_link_libraries(r-type_client PRIVATE
            sfml-system
            sfml-window
            sfml-graphics
            sfml-audio
            r-type-shared
            asio::asio

            # Ajout explicite des frameworks Apple pour OpenAL
            "-framework CoreAudio"
            "-framework AudioToolbox"
            "-framework CoreFoundation"   # Souvent utile
            # "-framework CoreServices"   # Ajoutez au besoin
    )
endif()


# Options de compilation
if(MSVC)
    target_compile_options(r-type_client PRIVATE /W4)
else()
    target_compile_options(r-type_client PRIVATE -Wall -Wextra)
endif()

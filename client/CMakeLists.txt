cmake_minimum_required(VERSION 3.29)

# Recherche de SFML
find_package(SFML COMPONENTS system window graphics audio CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)

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
        menu/TextInput.cpp
        menu/TextInput.hpp
        utils/UiHelpers.hpp
)

# Création de l'exécutable selon la plateforme
if(APPLE)
    add_executable(r-type_client ${CLIENT_SOURCES})
else()
    add_executable(r-type_client WIN32 ${CLIENT_SOURCES})
endif()

# Configuration des assets
set(ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")

add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${ASSETS_DIR}"
)

# Définitions de compilation
target_compile_definitions(r-type_client PRIVATE
        ASSETS_PATH="${ASSETS_DIR}"
)

# Include directories
target_include_directories(r-type_client PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
)

# Configuration spécifique à la plateforme pour le linking
if(WIN32)
    target_link_libraries(r-type_client PRIVATE
            sfml-system
            sfml-window
            sfml-graphics
            sfml-audio
            sfml-main
            r-type-shared
            asio::asio
            OpenAL::OpenAL
            opengl32
            winmm
            gdi32
            freetype
            ws2_32
            wsock32
            avrt
            "Avrt.lib"
            "Ole32.lib"
            "OleAut32.lib"
    )
elseif(APPLE)
    target_link_libraries(r-type_client PRIVATE
            sfml-system
            sfml-window
            sfml-graphics
            sfml-audio
            r-type-shared
            asio::asio
            "-framework CoreAudio"
            "-framework AudioToolbox"
            "-framework CoreFoundation"
            "-framework CoreVideo"
            "-framework OpenGL"
    )
else() # Linux
    target_link_libraries(r-type_client PRIVATE
            sfml-system
            sfml-window
            sfml-graphics
            sfml-audio
            r-type-shared
            asio::asio
    )
endif()

# Options de compilation
if(MSVC)
    target_compile_options(r-type_client PRIVATE /W4)
else()
    target_compile_options(r-type_client PRIVATE -Wall -Wextra)
endif()
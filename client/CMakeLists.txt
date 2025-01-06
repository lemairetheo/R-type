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
        systems/BackgroundSystem.cpp
        systems/BackgroundSystem.hpp
        menu/button/Button.hpp
        menu/button/Button.cpp
        menu/Inputs.hpp
        menu/Menu.hpp
        menu/Menu.cpp
)

add_executable(r-type_client ${CLIENT_SOURCES})

set(ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")

add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${ASSETS_DIR}"
)

target_compile_definitions(r-type_client PRIVATE
        ASSETS_PATH="${ASSETS_DIR}"
)

target_include_directories(r-type_client
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(r-type_client
        PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        r-type-shared
)

if(UNIX)
    target_compile_options(r-type_client
            PRIVATE
            -Wall
            -Wextra
    )
endif()

if(MSVC)
    target_compile_options(r-type_client
            PRIVATE
            /W4
    )
endif()
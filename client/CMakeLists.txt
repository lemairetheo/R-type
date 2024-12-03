add_executable(r-type_client
        main.cpp
        RTypeClientGraphics.hpp
        RTypeClientGraphics.cpp
)

target_include_directories(r-type_client
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(r-type_client
        PRIVATE
        r-type-shared
        sfml-graphics
        sfml-window
        sfml-system
)

# Platform specific configurations
if(WIN32)
    target_link_libraries(r-type_client
            PRIVATE
            wsock32
            ws2_32
    )
endif()

if(UNIX)
    target_compile_options(r-type_client
            PRIVATE
            -Wall
            -Wextra
    )
endif()
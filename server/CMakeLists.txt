find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(r-type_server
        main.cpp
        network/NetworkManager.cpp
        network/NetworkManager.hpp
        game/GameEngine.hpp
        game/GameEngine.cpp
        manager/Manager.cpp
        manager/Manager.hpp
        database/DatabaseManager.cpp
        database/DatabaseManager.hpp
        database/ScoreRepository.cpp
        database/ScoreRepository.hpp
        database/UserRepository.cpp
        database/UserRepository.hpp
)

target_include_directories(r-type_server
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(r-type_server
        PRIVATE
        r-type-shared
        asio::asio
        unofficial::sqlite3::sqlite3
)

# Platform specific configurations
if(WIN32)
    target_link_libraries(r-type_server
            PRIVATE
            wsock32
            ws2_32
    )
endif()

if(UNIX)
    target_compile_options(r-type_server
            PRIVATE
            -Wall
            -Wextra
    )
endif()

cmake_minimum_required(VERSION 3.29)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")
    find_package(Git REQUIRED)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} clone https://github.com/Microsoft/vcpkg.git
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_RESULT
    )
endif()


# Configuration de vcpkg - AJOUTEZ CECI AU DÉBUT
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
endif()

project(r-type)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FLAG CPP
if(CMAKE_COMPILER_IS_GNUCXX)
    set(COMPILER_TYPE "gcc")
    add_compile_options(
            "-Wall"
            "-Wextra"
            "-pedantic"
            "-fpic"
    )
endif()

find_package(asio CONFIG REQUIRED)

# Doxygen configuration
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else()
    message(WARNING "Doxygen not found, documentation will not be generated.")
endif()


# Add components
add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(client)
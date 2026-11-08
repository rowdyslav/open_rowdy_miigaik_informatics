cmake_minimum_required(VERSION 3.22)
project(platformer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(SFML_STATIC_LIBRARIES ON)
add_compile_definitions(SFML_STATIC)
if(MINGW)
    add_link_options(-static -static-libgcc -static-libstdc++)
endif()

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(platformer
    main.cpp
    GameScene.cpp
    GameView.cpp
    TmxLevel.cpp
    tinyxml2.cpp
)
target_include_directories(platformer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(platformer PRIVATE SFML::Graphics SFML::Window SFML::System)


set(APP_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set_target_properties(platformer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${APP_BIN_DIR})
add_custom_command(TARGET platformer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${APP_BIN_DIR}/res
)

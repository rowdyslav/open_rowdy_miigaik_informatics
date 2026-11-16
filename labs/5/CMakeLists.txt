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

set(PLATFORMER_SOURCES
    main.cpp
    GameScene.cpp
    GameView.cpp
    TmxLevel.cpp
    tinyxml2.cpp
)

# Генерирует C++-массивы при настройке проекта. Изменение любого ресурса
# автоматически запускает повторную настройку и обновляет автономный EXE.
set(EMBEDDED_RESOURCES_CPP ${CMAKE_CURRENT_BINARY_DIR}/embedded/EmbeddedResources.cpp)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/embedded)
file(WRITE ${EMBEDDED_RESOURCES_CPP}
    "#include \"EmbeddedResources.h\"\n\nnamespace EmbeddedResources {\n"
)

function(embed_resource symbol filepath)
    file(READ ${filepath} resource_hex HEX)
    string(REGEX REPLACE "(..)" "0x\\1," resource_bytes "${resource_hex}")
    file(APPEND ${EMBEDDED_RESOURCES_CPP}
        "static const unsigned char ${symbol}_BYTES[] = {${resource_bytes}};\n"
        "const EmbeddedResource ${symbol}{${symbol}_BYTES, sizeof(${symbol}_BYTES)};\n"
    )
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${filepath})
endfunction()

embed_resource(LEVEL ${CMAKE_CURRENT_SOURCE_DIR}/res/platformer.tmx)
embed_resource(TILESET ${CMAKE_CURRENT_SOURCE_DIR}/res/tileset.png)
embed_resource(FONT ${CMAKE_CURRENT_SOURCE_DIR}/res/arial.ttf)
file(APPEND ${EMBEDDED_RESOURCES_CPP} "} // namespace EmbeddedResources\n")

# WIN32 выбирает GUI-подсистему Windows: отдельная консоль не создаётся.
add_executable(platformer WIN32 ${PLATFORMER_SOURCES} ${EMBEDDED_RESOURCES_CPP})
target_include_directories(platformer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(platformer PRIVATE SFML::Graphics SFML::Window SFML::System)

# Единственный EXE содержит все ресурсы и запускается без каталога res/.
set_target_properties(platformer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

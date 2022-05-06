cmake_minimum_required(VERSION 3.16)

project(vkEasyGraphicsModel)

file(GLOB_RECURSE vkEasyGraphicsModel_SOURCES
    "*.cpp"
    "*.c"
)

file(GLOB_RECURSE vkEasyGraphicsModel_HEADERS
    "*.hpp"
    "*.h"
)

file(GLOB_RECURSE vkEasyGraphicsModel_SHADERS
    "*.vert"
    "*.frag"
)

source_group(headers FILES ${vkEasyGraphicsModel_HEADERS})
source_group(sources FILES ${vkEasyGraphicsModel_SOURCES})

add_executable(${PROJECT_NAME} ${vkEasyGraphicsModel_HEADERS} ${vkEasyGraphicsModel_SOURCES} ${vkEasyGraphicsModel_SHADERS})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/shader.vert" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/shader.frag" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/viking_room.obj" "${CMAKE_CURRENT_BINARY_DIR}/viking_room.obj" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/viking_room.png" "${CMAKE_CURRENT_BINARY_DIR}/viking_room.png" COPYONLY)

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)

cmake_minimum_required(VERSION 3.16)

project(vkEasyGraphicsTexture)

file(GLOB_RECURSE vkEasyGraphicsTexture_SOURCES
    "*.cpp"
    "*.c"
)

file(GLOB_RECURSE vkEasyGraphicsTexture_HEADERS
    "*.hpp"
    "*.h"
)

file(GLOB_RECURSE vkEasyGraphicsTexture_SHADERS
    "*.vert"
    "*.frag"
)

source_group(headers FILES ${vkEasyGraphicsTexture_HEADERS})
source_group(sources FILES ${vkEasyGraphicsTexture_SOURCES})

add_executable(${PROJECT_NAME} ${vkEasyGraphicsTexture_HEADERS} ${vkEasyGraphicsTexture_SOURCES} ${vkEasyGraphicsTexture_SHADERS})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/shader.vert" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/shader.frag" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/texture.jpg" "${CMAKE_CURRENT_BINARY_DIR}/texture.jpg" COPYONLY)

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)

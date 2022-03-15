cmake_minimum_required(VERSION 3.16)

project(vkEasyGraphics)

file(GLOB_RECURSE vkEasyGraphics_SOURCES
    "*.cpp"
    "*.c"
)

file(GLOB_RECURSE vkEasyGraphics_HEADERS
    "*.hpp"
    "*.h"
)

file(GLOB_RECURSE vkEasyGraphics_SHADERS
    "*.vert"
    "*.frag"
)

source_group(headers FILES ${vkEasyGraphics_HEADERS})
source_group(sources FILES ${vkEasyGraphics_SOURCES})

add_executable(${PROJECT_NAME} ${vkEasyGraphics_HEADERS} ${vkEasyGraphics_SOURCES} ${vkEasyGraphics_SHADERS})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/shader.vert" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/shader.frag" COPYONLY)

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)

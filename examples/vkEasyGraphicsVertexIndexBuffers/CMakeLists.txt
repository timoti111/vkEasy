cmake_minimum_required(VERSION 3.16)

project(vkEasyGraphicsVertexIndexBuffers)

file(GLOB_RECURSE vkEasyGraphicsVertexIndexBuffers_SOURCES
    "*.cpp"
    "*.c"
)

file(GLOB_RECURSE vkEasyGraphicsVertexIndexBuffers_HEADERS
    "*.hpp"
    "*.h"
)

file(GLOB_RECURSE vkEasyGraphicsVertexIndexBuffers_SHADERS
    "*.vert"
    "*.frag"
)

source_group(headers FILES ${vkEasyGraphicsVertexIndexBuffers_HEADERS})
source_group(sources FILES ${vkEasyGraphicsVertexIndexBuffers_SOURCES})

add_executable(${PROJECT_NAME} ${vkEasyGraphicsVertexIndexBuffers_HEADERS} ${vkEasyGraphicsVertexIndexBuffers_SOURCES} ${vkEasyGraphicsVertexIndexBuffers_SHADERS})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/shader.vert" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/shader.frag" COPYONLY)

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)

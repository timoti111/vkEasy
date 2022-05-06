cmake_minimum_required(VERSION 3.16)

project(vkEasyGraphicsDepthBuffer)

file(GLOB_RECURSE vkEasyGraphicsDepthBuffer_SOURCES
    "*.cpp"
    "*.c"
)

file(GLOB_RECURSE vkEasyGraphicsDepthBuffer_HEADERS
    "*.hpp"
    "*.h"
)

file(GLOB_RECURSE vkEasyGraphicsDepthBuffer_SHADERS
    "*.vert"
    "*.frag"
)

source_group(headers FILES ${vkEasyGraphicsDepthBuffer_HEADERS})
source_group(sources FILES ${vkEasyGraphicsDepthBuffer_SOURCES})

add_executable(${PROJECT_NAME} ${vkEasyGraphicsDepthBuffer_HEADERS} ${vkEasyGraphicsDepthBuffer_SOURCES} ${vkEasyGraphicsDepthBuffer_SHADERS})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/shader.vert" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/shader.frag" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/texture.jpg" "${CMAKE_CURRENT_BINARY_DIR}/texture.jpg" COPYONLY)

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)

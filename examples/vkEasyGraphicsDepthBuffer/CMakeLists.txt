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

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/Release/shader.vert" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/Release/shader.frag" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/texture.jpg" "${CMAKE_CURRENT_BINARY_DIR}/Release/texture.jpg" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/Debug/shader.vert" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/Debug/shader.frag" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/texture.jpg" "${CMAKE_CURRENT_BINARY_DIR}/Debug/texture.jpg" COPYONLY)
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/shader.vert" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/shader.frag" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/texture.jpg" "${CMAKE_CURRENT_BINARY_DIR}/texture.jpg" COPYONLY)

source_group(headers FILES ${vkEasyGraphicsDepthBuffer_HEADERS})
source_group(sources FILES ${vkEasyGraphicsDepthBuffer_SOURCES})

add_executable(${PROJECT_NAME} ${vkEasyGraphicsDepthBuffer_HEADERS} ${vkEasyGraphicsDepthBuffer_SOURCES} ${vkEasyGraphicsDepthBuffer_SHADERS})

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)

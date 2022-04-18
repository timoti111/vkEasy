cmake_minimum_required(VERSION 3.16)

project(vkEasyGraphicsUniformBuffers)

file(GLOB_RECURSE vkEasyGraphicsUniformBuffers_SOURCES
    "*.cpp"
    "*.c"
)

file(GLOB_RECURSE vkEasyGraphicsUniformBuffers_HEADERS
    "*.hpp"
    "*.h"
)

file(GLOB_RECURSE vkEasyGraphicsUniformBuffers_SHADERS
    "*.vert"
    "*.frag"
)

source_group(headers FILES ${vkEasyGraphicsUniformBuffers_HEADERS})
source_group(sources FILES ${vkEasyGraphicsUniformBuffers_SOURCES})

add_executable(${PROJECT_NAME} ${vkEasyGraphicsUniformBuffers_HEADERS} ${vkEasyGraphicsUniformBuffers_SOURCES} ${vkEasyGraphicsUniformBuffers_SHADERS})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/shader.vert" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/shader.frag" COPYONLY)

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)

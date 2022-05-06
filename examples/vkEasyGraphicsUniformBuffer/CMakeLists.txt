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

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/Release/shader.vert" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/Release/shader.frag" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/Debug/shader.vert" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/Debug/shader.frag" COPYONLY)
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" "${CMAKE_CURRENT_BINARY_DIR}/shader.vert" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" "${CMAKE_CURRENT_BINARY_DIR}/shader.frag" COPYONLY)

source_group(headers FILES ${vkEasyGraphicsUniformBuffers_HEADERS})
source_group(sources FILES ${vkEasyGraphicsUniformBuffers_SOURCES})

add_executable(${PROJECT_NAME} ${vkEasyGraphicsUniformBuffers_HEADERS} ${vkEasyGraphicsUniformBuffers_SOURCES} ${vkEasyGraphicsUniformBuffers_SHADERS})

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)

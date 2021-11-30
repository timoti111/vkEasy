cmake_minimum_required(VERSION 3.16)

project(vkEasyTesting)

file(GLOB_RECURSE vkEasyTesting_SOURCES
    "*.cpp"
    "*.c"
)

file(GLOB_RECURSE vkEasyTesting_HEADERS
    "*.hpp"
    "*.h"
)

file(GLOB_RECURSE vkEasyTesting_SHADERS
    "*.comp"
)

source_group(headers FILES ${vkEasyTesting_HEADERS})
source_group(sources FILES ${vkEasyTesting_SOURCES})

add_executable(${PROJECT_NAME} ${vkEasyTesting_HEADERS} ${vkEasyTesting_SOURCES} ${vkEasyTesting_SHADERS})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/headless.comp" "${CMAKE_CURRENT_BINARY_DIR}/headless.comp" COPYONLY)

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)

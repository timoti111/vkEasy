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

source_group(headers FILES ${vkEasyTesting_HEADERS})
source_group(sources FILES ${vkEasyTesting_SOURCES})

add_executable(${PROJECT_NAME} ${vkEasyTesting_HEADERS} ${vkEasyTesting_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)

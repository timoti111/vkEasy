cmake_minimum_required(VERSION 3.16)

project(vkEasyCompute)

file(GLOB_RECURSE vkEasyCompute_SOURCES
    "*.cpp"
    "*.c"
)

file(GLOB_RECURSE vkEasyCompute_HEADERS
    "*.hpp"
    "*.h"
)

file(GLOB_RECURSE vkEasyCompute_SHADERS
    "*.comp"
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/headless.comp" "${CMAKE_CURRENT_BINARY_DIR}/Release/headless.comp" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/headless.comp" "${CMAKE_CURRENT_BINARY_DIR}/Debug/headless.comp" COPYONLY)
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/headless.comp" "${CMAKE_CURRENT_BINARY_DIR}/headless.comp" COPYONLY)

source_group(headers FILES ${vkEasyCompute_HEADERS})
source_group(sources FILES ${vkEasyCompute_SOURCES})

add_executable(${PROJECT_NAME} ${vkEasyCompute_HEADERS} ${vkEasyCompute_SOURCES} ${vkEasyCompute_SHADERS})
target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)



cmake_minimum_required(VERSION 3.16)

project(vkGraphics)
find_package (Vulkan REQUIRED)

file(GLOB_RECURSE vkGraphics_SOURCES
    "*.cpp"
    "*.c"
)

file(GLOB_RECURSE vkGraphics_HEADERS
    "*.hpp"
    "*.h"
)

file(GLOB_RECURSE vkGraphics_SHADERS
    "shaders/*.spv"
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shaders/frag.spv" "${CMAKE_CURRENT_BINARY_DIR}/Release/shaders/frag.spv" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shaders/vert.spv" "${CMAKE_CURRENT_BINARY_DIR}/Release/shaders/vert.spv" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shaders/frag.spv" "${CMAKE_CURRENT_BINARY_DIR}/Debug/shaders/frag.spv" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shaders/vert.spv" "${CMAKE_CURRENT_BINARY_DIR}/Debug/shaders/vert.spv" COPYONLY)
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shaders/frag.spv" "${CMAKE_CURRENT_BINARY_DIR}/shaders/frag.spv" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shaders/vert.spv" "${CMAKE_CURRENT_BINARY_DIR}/shaders/vert.spv" COPYONLY)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DNOMINMAX -DVK_USE_PLATFORM_WIN32_KHR)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
else()
    message(FATAL_ERROR, "Vulkan-Hpp: unhandled platform for samples!")
endif()

source_group(headers FILES ${vkGraphics_HEADERS})
source_group(sources FILES ${vkGraphics_SOURCES})

add_executable(${PROJECT_NAME} ${vkGraphics_HEADERS} ${vkGraphics_SOURCES} ${vkGraphics_SHADERS})
target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan vkEasy::vkEasy)

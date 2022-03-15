cmake_minimum_required(VERSION 3.16)

project(vkCompute)

file(GLOB_RECURSE vkCompute_SOURCES
    "*.cpp"
    "*.c"
)

file(GLOB_RECURSE vkCompute_HEADERS
    "*.hpp"
    "*.h"
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DNOMINMAX -DVK_USE_PLATFORM_WIN32_KHR)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
else()
    message(FATAL_ERROR, "Vulkan-Hpp: unhandled platform for samples!")
endif()


source_group(headers FILES ${vkCompute_HEADERS})
source_group(sources FILES ${vkCompute_SOURCES})

add_executable(${PROJECT_NAME} ${vkCompute_HEADERS} ${vkCompute_SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} PRIVATE vkEasy::vkEasy)

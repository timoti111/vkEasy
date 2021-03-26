cmake_minimum_required(VERSION 3.16)

project(vkEasy)

set(ENV{CMAKE_BUILD_PARALLEL_LEVEL})

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/extras/Vulkan-Hpp")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/extras/Vulkan-Hpp/Vulkan-Headers/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/extras/VMA")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DNOMINMAX -DVK_USE_PLATFORM_WIN32_KHR)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
else()
    message(FATAL_ERROR, "Vulkan-Hpp: unhandled platform for samples!")
endif()

file(GLOB_RECURSE VK_EASY_SOURCES
    "src/*.hpp"
    "src/*.h"
    "src/*.cpp"
    "src/*.c"
)
file(GLOB_RECURSE VK_EASY_INCLUDES
    "include/*.hpp"
    "include/*.h"
)

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")

add_library(${PROJECT_NAME} ${VK_EASY_SOURCES} ${VK_EASY_INCLUDES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(${PROJECT_NAME} glslang glslang-default-resource-limits spirv-cross-core SPIRV ${CMAKE_DL_LIBS})
target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /permissive-)
else(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
add_subdirectory(extras)
add_subdirectory(examples)

cmake_minimum_required(VERSION 3.16)

project(vkEasy)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(extras)
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

SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
SET(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
SET(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")

add_library(${PROJECT_NAME} ${VK_EASY_SOURCES} ${VK_EASY_INCLUDES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

target_link_libraries(${PROJECT_NAME} glslang glslang-default-resource-limits SPIRV Vulkan::Vulkan)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /permissive-)
else(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

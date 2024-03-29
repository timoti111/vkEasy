cmake_minimum_required(VERSION 3.16)

project(vkEasy)

set(ENV{CMAKE_BUILD_PARALLEL_LEVEL})

set(CMAKE_CXX_STANDARD 20)

option (VK_EASY_BUILD_EXAMPLES "Build the GLFW example programs" ON)
option (VK_EASY_BUILD_WITH_LOCAL_VULKAN_HPP "Build with local Vulkan headers" ON)
option (VK_EASY_BUILD_ONLY_DYNAMIC "Build only dynamic. Required in case the Vulkan SDK is not available" OFF)

if(NOT (VK_EASY_BUILD_ONLY_DYNAMIC AND VK_EASY_BUILD_WITH_LOCAL_VULKAN_HPP))
  find_package(Vulkan REQUIRED)
endif()

if (VK_EASY_BUILD_WITH_LOCAL_VULKAN_HPP)
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/extras/Vulkan-Hpp")
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/extras/Vulkan-Hpp/Vulkan-Headers/include")
else()
  include_directories("${Vulkan_INCLUDE_DIRS}")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/extras/VulkanMemoryAllocator/include")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DNOMINMAX -DVK_USE_PLATFORM_WIN32_KHR)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
else()
    message(FATAL_ERROR, "Vulkan-Hpp: unhandled platform for samples!")
endif()

set(VK_EASY_SOURCES
    "src/vkEasy/nodes/base/Node.cpp"
    "src/vkEasy/nodes/base/PipelineNode.cpp"
    "src/vkEasy/nodes/ComputeNode.cpp"
    "src/vkEasy/nodes/GraphicsNode.cpp"
    "src/vkEasy/nodes/MemoryCopyNode.cpp"
    "src/vkEasy/resources/base/Buffer.cpp"
    "src/vkEasy/resources/base/Image.cpp"
    "src/vkEasy/resources/base/Resource.cpp"
    "src/vkEasy/resources/ColorAttachment.cpp"
    "src/vkEasy/resources/DepthStencilBuffer.cpp"
    "src/vkEasy/resources/IndexBuffer.cpp"
    "src/vkEasy/resources/StagingBuffer.cpp"
    "src/vkEasy/resources/StorageBuffer.cpp"
    "src/vkEasy/resources/SwapChainImage.cpp"
    "src/vkEasy/resources/TextureImage.cpp"
    "src/vkEasy/resources/UniformBuffer.cpp"
    "src/vkEasy/resources/VertexBuffer.cpp"
    "src/vkEasy/Context.cpp"
    "src/vkEasy/Device.cpp"
    "src/vkEasy/Error.cpp"
    "src/vkEasy/Framebuffer.cpp"
    "src/vkEasy/GLFWWindow.cpp"
    "src/vkEasy/global.cpp"
    "src/vkEasy/Graph.cpp"
    "src/vkEasy/MemoryAllocator.cpp"
    "src/vkEasy/ShaderStage.cpp"
    "src/vkEasy/WSI.cpp"
)
set(VK_EASY_INCLUDES
    "include/vkEasy/nodes/base/Node.h"
    "include/vkEasy/nodes/base/PipelineNode.h"
    "include/vkEasy/nodes/ComputeNode.h"
    "include/vkEasy/nodes/GraphicsNode.h"
    "include/vkEasy/nodes/MemoryCopyNode.h"
    "include/vkEasy/nodes/Nodes.h"
    "include/vkEasy/resources/base/Buffer.h"
    "include/vkEasy/resources/base/Image.h"
    "include/vkEasy/resources/base/Resource.h"
    "include/vkEasy/resources/ColorAttachment.h"
    "include/vkEasy/resources/DepthStencilBuffer.h"
    "include/vkEasy/resources/IndexBuffer.h"
    "include/vkEasy/resources/Resources.h"
    "include/vkEasy/resources/StagingBuffer.h"
    "include/vkEasy/resources/StorageBuffer.h"
    "include/vkEasy/resources/SwapChainImage.h"
    "include/vkEasy/resources/TextureImage.h"
    "include/vkEasy/resources/UniformBuffer.h"
    "include/vkEasy/resources/VertexBuffer.h"
    "include/vkEasy/Context.h"
    "include/vkEasy/Device.h"
    "include/vkEasy/Error.h"
    "include/vkEasy/Framebuffer.h"
    "include/vkEasy/GLFWWindow.h"
    "include/vkEasy/global.h"
    "include/vkEasy/Graph.h"
    "include/vkEasy/MemoryAllocator.h"
    "include/vkEasy/ShaderStage.h"
    "include/vkEasy/Utils.h"
    "include/vkEasy/WSI.h"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")

add_library(${PROJECT_NAME} ${VK_EASY_SOURCES} ${VK_EASY_INCLUDES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(${PROJECT_NAME}  PUBLIC ${shaderc_SOURCE_DIR}/libshaderc_util/include)
target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
target_link_libraries(${PROJECT_NAME} shaderc ${CMAKE_DL_LIBS} glm)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-) # /WX
else(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif(MSVC)



set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

add_subdirectory(extras)
if (VK_EASY_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()
target_link_libraries(${PROJECT_NAME} glfw)

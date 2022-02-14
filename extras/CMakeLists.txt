cmake_minimum_required(VERSION 3.16)

add_subdirectory("SPIRV-Headers")
add_subdirectory("SPIRV-Tools")
add_subdirectory("glslang")
add_subdirectory("SPIRV-Cross")
add_subdirectory("shaderc")
add_subdirectory("VulkanMemoryAllocator")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("glfw")

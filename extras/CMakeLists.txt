cmake_minimum_required(VERSION 3.16)

FILE (GLOB extras ${CMAKE_CURRENT_SOURCE_DIR}/*)
FOREACH( extra ${extras} )
    if(IS_DIRECTORY ${extra})
        if(EXISTS ${extra}/CMakeLists.txt)
            if(NOT ${extra} MATCHES "Vulkan-Hpp$")
            string(REGEX REPLACE "^.*/([^/]*)$" "\\1" LINK_NAME ${extra})
            add_subdirectory(${LINK_NAME})
            endif()
        endif()
    endif()
ENDFOREACH(extra ${extras})

add_subdirectory("Vulkan-Hpp/glslang")


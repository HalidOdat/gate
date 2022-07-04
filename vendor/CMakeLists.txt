cmake_minimum_required(VERSION 3.0...3.20)

# glfw
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_INSTALL  OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)

# glm
add_subdirectory(glm)

# glad
set(This glad)

add_library(${This} STATIC glad/src/glad.c)

set_target_properties(${This} PROPERTIES
    FOLDER libraries
)

target_include_directories(${This} PUBLIC glad/include)

# stb image
set(This stb)

add_library(${This} INTERFACE)

target_include_directories(${This} INTERFACE stb)

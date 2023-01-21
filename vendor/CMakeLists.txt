cmake_minimum_required(VERSION 3.0...3.20)

if (NOT DEFINED WEB)
  # glfw
  set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_INSTALL  OFF CACHE BOOL "" FORCE)

  add_subdirectory(glfw)
else()
  add_library(glfw INTERFACE)
  target_link_options(glfw
    INTERFACE
      -sUSE_GLFW=3
  )
endif()

add_library(glfw::glfw ALIAS glfw)

# glm
add_subdirectory(glm)

# glad
set(This glad)

add_library(${This} STATIC glad/src/glad.c)

set_target_properties(${This} PROPERTIES
    FOLDER libraries
)

target_include_directories(${This} PUBLIC glad/include)

add_library(${This}::${This} ALIAS ${This})

# stb image
set(This stb)

add_library(${This} INTERFACE)

target_include_directories(${This} INTERFACE stb)

add_library(${This}::${This} ALIAS ${This})

# Assimp image
set(BUILD_SHARED_LIBS         OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL            OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

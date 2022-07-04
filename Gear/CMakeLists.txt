cmake_minimum_required(VERSION 3.0..3.20 FATAL_ERROR)

set(This Gear)

set(Headers
    include/${This}/Window.hpp
)

set(Sources
    src/Window.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})

target_include_directories(${This} PUBLIC include)

target_link_libraries(${This} PUBLIC
  glfw
  glad
  glm::glm
  stb
)

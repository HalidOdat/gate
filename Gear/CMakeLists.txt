cmake_minimum_required(VERSION 3.0..3.20 FATAL_ERROR)

set(This Gear)

set(Headers
    include/${This}/Core/Type.hpp
    include/${This}/Core.hpp
    include/${This}/KeyCode.hpp
    include/${This}/Window.hpp
    include/${This}/Application.hpp
)

set(Sources
    src/Core.cpp
    src/Window.cpp
    src/Application.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})

target_include_directories(${This} PUBLIC include)

target_link_libraries(${This} PUBLIC
  glfw
  glad
  glm::glm
  stb
)

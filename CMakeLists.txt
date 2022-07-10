cmake_minimum_required(VERSION 3.0..3.20 FATAL_ERROR)

set(This Game)

project(${This}
  VERSION 0.1.0
  LANGUAGES CXX C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Sources
  src/Core/Type.hpp
  src/Core/Assert.hpp
  src/Core/Assert.cpp
  src/Core/Log.hpp
  src/Core/Log.cpp
  src/Core/KeyCode.hpp
  src/VertexBuffer.hpp
  src/VertexBuffer.cpp
  src/IndexBuffer.hpp
  src/IndexBuffer.cpp
  src/VertexArray.hpp
  src/VertexArray.cpp
  src/Window.hpp
  src/Window.cpp
  src/Application.hpp
  src/Application.cpp
  src/main.cpp
)

add_executable(${This} ${Sources})

target_include_directories(${This} PUBLIC src)

target_link_libraries(${This} PUBLIC
  glfw
  glad
  glm::glm
  stb
)

add_subdirectory(vendor)
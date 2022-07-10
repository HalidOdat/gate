cmake_minimum_required(VERSION 3.0..3.20 FATAL_ERROR)

set(This Game)

project(${This}
  VERSION 0.1.0
  LANGUAGES CXX C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers
    include/Core/Assert.hpp
    include/Core/Log.hpp
    include/Core/Type.hpp
    include/Core/KeyCode.hpp
    include/VertexBuffer.hpp
    include/VertexArray.hpp
    include/Window.hpp
    include/Application.hpp
)

set(Sources
    src/Core/Log.cpp
    src/VertexBuffer.cpp
    src/VertexArray.cpp
    src/Window.cpp
    src/Application.cpp
    src/main.cpp
)

add_executable(${This} ${Sources} ${Headers})

target_include_directories(${This} PUBLIC include)

target_link_libraries(${This} PUBLIC
  glfw
  glad
  glm::glm
  stb
)

add_subdirectory(vendor)
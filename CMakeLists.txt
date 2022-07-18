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
  
  src/Events/KeyCode.hpp
  src/Events/Event.hpp
  src/Events/Event.cpp
  src/Events/KeyEvent.hpp
  src/Events/MouseEvent.hpp
  src/Events/WindowEvent.hpp
  
  src/Renderer/VertexBuffer.hpp
  src/Renderer/VertexBuffer.cpp
  src/Renderer/IndexBuffer.hpp
  src/Renderer/IndexBuffer.cpp
  src/Renderer/VertexArray.hpp
  src/Renderer/VertexArray.cpp
  src/Renderer/Shader.hpp
  src/Renderer/Shader.cpp
  src/Renderer/Texture.hpp
  src/Renderer/Texture.cpp
  src/Renderer/Camera.hpp
  src/Renderer/Camera.cpp
  
  src/Layers/Layer.hpp
  src/Layers/LayerStack.hpp
  src/Layers/LayerStack.cpp
  src/Layers/UILayer.hpp
  src/Layers/UILayer.cpp
  src/Layers/ExampleLayer.hpp
  src/Layers/ExampleLayer.cpp
  
  src/Window.hpp
  src/Window.cpp
  src/Application.hpp
  src/Application.cpp
  src/Game.hpp
  src/Game.cpp
  
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
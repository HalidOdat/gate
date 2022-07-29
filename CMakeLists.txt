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
  src/Core/Math.hpp
  src/Core/KeyCode.hpp
  src/Core/Input.hpp
  src/Core/Input.cpp
  src/Core/Timestep.hpp
  src/Core/Timestep.cpp
  
  src/Utils/Color.hpp

  src/Events/Event.hpp
  src/Events/Event.cpp
  src/Events/KeyEvent.hpp
  src/Events/MouseEvent.hpp
  src/Events/WindowEvent.hpp
  
  src/Resource/Manager.hpp
  src/Resource/Manager.cpp
  src/Resource/Resource.hpp
  src/Resource/Resource.cpp
  src/Resource/Texture.hpp
  src/Resource/Texture.cpp
  src/Resource/Shader.hpp
  src/Resource/Shader.cpp
  src/Resource/Mesh.hpp
  src/Resource/Mesh.cpp

  src/Ecs/Entity.hpp
  src/Ecs/Component.hpp
  src/Ecs/Component.cpp
  src/Ecs/Ecs.hpp
  src/Ecs/Ecs.cpp
  
  src/Renderer/VertexBuffer.hpp
  src/Renderer/VertexBuffer.cpp
  src/Renderer/IndexBuffer.hpp
  src/Renderer/IndexBuffer.cpp
  src/Renderer/VertexArray.hpp
  src/Renderer/VertexArray.cpp
  src/Renderer/Camera.hpp
  src/Renderer/Camera.cpp
  src/Renderer/CameraController.hpp
  src/Renderer/CameraController.cpp
  src/Renderer/Renderer.hpp
  src/Renderer/Renderer.cpp

  src/Layers/Layer.hpp
  src/Layers/LayerStack.hpp
  src/Layers/LayerStack.cpp
  src/Layers/GameLayer.hpp
  src/Layers/GameLayer.cpp
  src/Layers/ExampleLayer.hpp
  src/Layers/ExampleLayer.cpp

  src/Ui/Ui.hpp
  src/Ui/Ui.cpp

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
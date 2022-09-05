cmake_minimum_required(VERSION 3.0..3.20 FATAL_ERROR)

set(This Game)

project(${This}
  VERSION 0.1.0
  LANGUAGES CXX C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (${CMAKE_SYSTEM_NAME} STREQUAL Emscripten)
  set(WEB 1)
endif()

add_subdirectory(vendor)

set(GAME_ASSETS_DIRECTOY "${PROJECT_SOURCE_DIR}/assets")
message(STATUS "${This}: Assets directory path: ${GAME_ASSETS_DIRECTOY}")

add_executable(${This}
  src/Core/Type.hpp
  src/Core/Macro.hpp
  src/Core/Macro.cpp
  src/Core/Log.hpp
  src/Core/Log.cpp
  src/Core/Math.hpp
  src/Core/KeyCode.hpp
  src/Core/Input.hpp
  src/Core/Input.cpp
  src/Core/Timestep.hpp
  src/Core/Timestep.cpp
  src/Core/Base.hpp
  
  src/Utils/Color.hpp
  src/Utils/File.hpp
  src/Utils/File.cpp
  src/Utils/String.hpp
  src/Utils/String.cpp
  src/Utils/Memory/BumpAllocator.hpp
  src/Utils/Memory/BumpAllocator.cpp
  src/Utils/Parsing/Lexer.hpp
  src/Utils/Parsing/Lexer.cpp

  src/Debug/Profiler.hpp
  src/Debug/Profiler.cpp

  src/Serializer/Serializer.hpp
  src/Serializer/Serializer.cpp

  src/Events/Event.hpp
  src/Events/Event.cpp
  src/Events/KeyEvent.hpp
  src/Events/MouseEvent.hpp
  src/Events/WindowEvent.hpp
  
  src/Resource/Manager.hpp
  src/Resource/Manager.cpp
  src/Resource/Resource.hpp
  src/Resource/Resource.cpp
  src/Resource/Factory.hpp
  src/Resource/Texture.hpp
  src/Resource/Texture.cpp
  src/Resource/Shader.hpp
  src/Resource/Shader.cpp
  src/Resource/Mesh.hpp
  src/Resource/Mesh.cpp
  src/Resource/UniformBuffer.hpp
  src/Resource/UniformBuffer.cpp

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
  src/Renderer/FrameBuffer.hpp
  src/Renderer/FrameBuffer.cpp
  src/Renderer/Camera.hpp
  src/Renderer/Camera.cpp
  src/Renderer/CameraController.hpp
  src/Renderer/CameraController.cpp
  src/Renderer/Material.hpp
  src/Renderer/Material.cpp
  src/Renderer/Renderer.hpp
  src/Renderer/Renderer.cpp
  src/Renderer/Renderer2D.hpp
  src/Renderer/Renderer2D.cpp
  src/Renderer/Renderer3D.hpp
  src/Renderer/Renderer3D.cpp

  src/Physics/Engine.hpp
  src/Physics/Engine.cpp

  src/Scene/Scene.hpp
  src/Scene/Scene.cpp
  src/Scene/Components.hpp
  src/Scene/Components.cpp
  src/Scene/Entity.hpp
  src/Scene/SceneSerializer.hpp
  src/Scene/SceneSerializer.cpp

  src/Layers/Layer.hpp
  src/Layers/LayerStack.hpp
  src/Layers/LayerStack.cpp
  src/Layers/GameLayer.hpp
  src/Layers/GameLayer.cpp
  src/Layers/EditorLayer.hpp
  src/Layers/EditorLayer.cpp

  src/Ui/Ui.hpp
  src/Ui/Ui.cpp

  src/Window.hpp
  src/Window.cpp
  src/Application.hpp
  src/Application.cpp

  src/Main.cpp
)

target_include_directories(${This} PUBLIC src)

if (MSVC)
  target_compile_options(${This}
    PRIVATE
      /W4
  )
else()
  target_compile_options(${This}
    PRIVATE
      -Wall -Wextra -pedantic
  )
endif()

if (DEFINED WEB)
  message(STATUS "${This}: Building for web")
  if (NOT UNIX)
    message(FATAL_ERROR "${This}: Building for web on windows could cause some errors. Please build in WSL")
  endif()

  target_link_options(${This}
    PUBLIC
      -sFULL_ES3=1
      -sUSE_WEBGL2=1
      -sMIN_WEBGL_VERSION=2
      -sMAX_WEBGL_VERSION=2
      -sALLOW_MEMORY_GROWTH=1
      --preload-file "${GAME_ASSETS_DIRECTOY}@assets"
  )
  set_target_properties(${This}
    PROPERTIES
      SUFFIX ".html"
  )
else()
  message(STATUS "${This}: Building for native")
endif()

target_link_libraries(${This} PUBLIC
  glfw::glfw
  glad::glad
  glm::glm
  stb::stb
)

target_precompile_headers(${This}
  PUBLIC
    # Common include 
    <Core/Base.hpp>

  PRIVATE
    # Standard library
    <cstdio>
    <cstdlib>
    <cstdint>
    <cstddef>
    <string>
    <string_view>
    <vector>
    <array>
    <memory>
    <optional>
    <fstream>
    <sstream>
    <map>
    <unordered_map>
    <type_traits>

    # vendor
    <glm/glm.hpp>
    <glad/glad.h>
)


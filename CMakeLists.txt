cmake_minimum_required(VERSION 3.0..3.20 FATAL_ERROR)

set(This gate)

project(${This}
  VERSION 0.1.0
  LANGUAGES CXX C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (${CMAKE_SYSTEM_NAME} STREQUAL Emscripten)
  set(WEB 1)
endif()

add_subdirectory(vendor)

set(ASSETS_DIRECTOY "${PROJECT_SOURCE_DIR}/assets")
message(STATUS "${This}: Assets directory path: ${ASSETS_DIRECTOY}")

add_executable(${This}
  src/Core/Type.hpp
  src/Core/Macro.hpp
  src/Core/Macro.cpp
  src/Core/Log.hpp
  src/Core/Log.cpp
  src/Core/KeyCode.hpp
  src/Core/Input.hpp
  src/Core/Input.cpp
  src/Core/Timestep.hpp
  src/Core/Timestep.cpp
  src/Core/Color.hpp
  src/Core/OpenGL.hpp
  src/Core/Base.hpp
  
  src/Utils/File.hpp
  src/Utils/File.cpp
  src/Utils/String.hpp
  src/Utils/String.cpp
  src/Utils/Parsing/Lexer.hpp
  src/Utils/Parsing/Lexer.cpp

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
  src/Resource/Factory.hpp
  src/Resource/Texture.hpp
  src/Resource/Texture.cpp
  src/Resource/Shader.hpp
  src/Resource/Shader.cpp
  src/Resource/UniformBuffer.hpp
  src/Resource/UniformBuffer.cpp

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
  src/Renderer/Renderer.hpp
  src/Renderer/Renderer.cpp

  src/Ui/Ui.hpp
  src/Ui/Ui.cpp

  src/Editor/Components/Component.hpp
  src/Editor/Components/Component.cpp
  src/Editor/Components/SwitchComponent.hpp
  src/Editor/Components/SwitchComponent.cpp
  src/Editor/Components/NotComponent.hpp
  src/Editor/Components/NotComponent.cpp
  src/Editor/Components/AndComponent.hpp
  src/Editor/Components/AndComponent.cpp
  src/Editor/Components/OrComponent.hpp
  src/Editor/Components/OrComponent.cpp
  src/Editor/Components.hpp
  src/Editor/Config.hpp
  src/Editor/Config.cpp
  src/Editor/Point.hpp
  src/Editor/Wire.hpp
  src/Editor/Wire.cpp
  src/Editor/Pin.hpp
  src/Editor/Connection.hpp
  src/Editor/EditorLayer.hpp
  src/Editor/EditorLayer.cpp

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
      -fno-exceptions
      -ggdb -fsanitize=address -fsanitize=undefined
      # -fsanitize=null
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
      --preload-file "${ASSETS_DIRECTOY}@assets"
  )
  set_target_properties(${This}
    PROPERTIES
      SUFFIX ".html"
  )
else()
  message(STATUS "${This}: Building for native")
endif()

target_link_options(${This}
  PRIVATE
    -fno-exceptions
    -fsanitize=address
    -fsanitize=undefined
)

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
    <unordered_map>
    <type_traits>

    # vendor
    <glm/glm.hpp>
    <glad/glad.h>
)

target_compile_definitions(${This}
  PUBLIC
    GATE_DEBUG_MODE=1
)

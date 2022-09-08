#pragma once

#include "Core/Math.hpp"
#include "Utils/Color.hpp"
#include "Resource/Manager.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/CameraController.hpp"

namespace Game {
  
  class Application;

  class Renderer {
    friend class Application;

  public:
    Renderer();

    static void enableBlending(bool  yes = true);
    static void enableDepthTest(bool yes = true);
    static void enableCullFace(bool  yes = true);

    static void begin3D(const PerspectiveCameraController& cameraController);
    static void submit(const Mesh::Handle& mesh, const Material::Handle& material, const Mat4& transform = Mat4(1.0f), u32 entityId = 0);

    static void flush();
    static void end();

    #if GAME_EDITOR
      static u32 readPixel(u32 x, u32 y);
    #endif

  private:
    static void waitAndRender();
    static void initialize();
    static void shutdown();
    static void invalidate(u32 width, u32 height);
  };

} // namespace Game

#include <array>
#include <cctype>
#include <algorithm>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Base.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Resource/Manager.hpp"
#include "Application.hpp"


namespace Game {

  // TODO: Use uniform buffer for camera and static data

  void Renderer::enableBlending(bool yes) {
    if (yes) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
  }

  void Renderer::enableDepthTest(bool yes) {
    if (yes) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }

  void Renderer::enableCullFace(bool yes) {
    if (yes) {
      glEnable(GL_CULL_FACE);
    } else {
      glDisable(GL_CULL_FACE);
    }
  }

  Renderer3D* renderer = nullptr;

  void Renderer::initialize() {
    renderer = new Renderer3D();
  }

  void Renderer::shutdown() {
    delete renderer;
    renderer = nullptr;
  }

  void Renderer::invalidate(u32 width, u32 height) {
    renderer->invalidate(width, height);
  }
  
  void Renderer::begin3D(const PerspectiveCameraController& cameraController) {
    renderer->begin3D(cameraController);
  }

  void Renderer::submit(const Mesh::Handle& mesh, const Material::Handle& material, const Mat4& transform, u32 entityId) {
    renderer->submit(mesh, material, transform, entityId);
  }
  
  void Renderer::waitAndRender() {
    renderer->waitAndRender();
  }

  void Renderer::end() {
    renderer->end();
  }

  #if GAME_EDITOR
    u32 Renderer::readPixel(u32 x, u32 y) {
      return renderer->readPixel(x, y);
    }

    void Renderer::setSelectedEntity(u32 entityId) {
      renderer->setSelectedEntity(entityId);
    }
  #endif

} // namespace Game

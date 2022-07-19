#include <glad/glad.h>


#include "Core/Log.hpp"
#include "Layers/UILayer.hpp"

#include "Renderer/Renderer.hpp"

namespace Game {

  UILayer::UILayer()
    : camera(-1.0f, 1.0f, -1.0f, 1.0f)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  }

  void UILayer::onAttach() {
    Logger::info("onAttach was called");
  }

  void UILayer::onDetach() {
    Logger::info("onDetach was called");
  }

  void UILayer::onUpdate() {
    Renderer::begin(this->camera);
    Renderer::drawSquare({-0.2f, 0.2f}, {0.2f, 0.2f});
    Renderer::drawSquare({0.2f, -0.2f}, {0.2f, 0.2f});
    Renderer::end();
  }

  void UILayer::onEvent(const Event& event) {
    event.dispatch(&UILayer::onWindowResizeEvent, this);
    event.dispatch(&UILayer::onKeyPressedEvent, this);
  }

  bool UILayer::onWindowResizeEvent(const WindowResizeEvent& event) {
    // TODO: resize camera
    return false;
  }

  bool UILayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    if (event.getKey() == Key::Up) {
      this->camera.offsetPosition({0.0f, 0.1f, 0.0f});
    } else if (event.getKey() == Key::Left) {
      this->camera.offsetRotation(0.1f);
    }
    return false;
  }

} // namespace Game
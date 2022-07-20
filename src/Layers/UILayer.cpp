#include <glad/glad.h>


#include "Core/Log.hpp"
#include "Renderer/Renderer.hpp"
#include "Ui/Ui.hpp"
#include "Layers/UILayer.hpp"


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
      Renderer::drawQuad({-0.5f, -0.5f}, {0.2f, 0.2f}, Color::RED);
      Renderer::drawQuad({0.5f, -0.5f}, {0.2f, 0.2f}, Color::GREEN);
      Renderer::drawQuad({0.5f,  0.5f}, {0.2f, 0.2f}, Color::BLACK);
      Renderer::drawQuad({-0.5f,  0.5f}, {0.2f, 0.2f}, Color::WHITE);
    Renderer::end();

    // Ui::begin({0.0f, 0.0f});
    // if (Ui::button(Color::GREEN)) {
    //   Logger::info("Button clicked!!!");
    // }
    // Ui::end();
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
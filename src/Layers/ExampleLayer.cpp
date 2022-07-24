#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>


#include "Core/Log.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Manager.hpp"
#include "Layers/ExampleLayer.hpp"

namespace Game {

  ExampleLayer::ExampleLayer()
  : camera{-1, 1, -1, 1}, texture{ResourceManager::loadTexture("thinking-emoji.png")}
  {}

  void ExampleLayer::onAttach() {
    Logger::info("ExampleLayer::onAttach was called");
  }

  void ExampleLayer::onDetach() {
    Logger::info("ExampleLayer::onDetach was called");
  }

  void ExampleLayer::onUpdate(Timestep ts) {
    Renderer::begin(this->camera);
    Renderer::drawQuad({0.0f, 1.0f, 0.0f}, {0.1f, 0.1f}, this->texture);
    Renderer::end();
  }

  void ExampleLayer::onUiRender(Ui& ui) {
    ui.begin({0.0f, 0.0f});
      if (ui.button({1.0f, 1.0f, 0.0f})) {
        Logger::info("Button clicked!!!");
      }
    ui.end();
  }

  void ExampleLayer::onEvent(const Event& event) {
    event.dispatch(&ExampleLayer::onKeyPressedEvent, this);
  }

  bool ExampleLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    if (event.getKey() == Key::R) {
      ResourceManager::reloadTextures();
    }
    return true;
  }

} // namespace Game
#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Manager.hpp"
#include "Layers/ExampleLayer.hpp"

namespace Game {

  ExampleLayer::ExampleLayer()
  : camera{-1, 1, -1, 1}, texture{ResourceManager::loadTexture("pixel_font_7x9.png", true)}
  {}

  void ExampleLayer::onAttach() {
    Logger::info("ExampleLayer::onAttach was called");
  }

  void ExampleLayer::onDetach() {
    Logger::info("ExampleLayer::onDetach was called");
  }

  void ExampleLayer::onUpdate(Timestep ts) {
    auto fps = std::to_string(1.0f / ts);
    Renderer::begin(this->camera);
    // Renderer::drawText(fps + "fps", {-0.98f, 0.98f, 0.0f}, {0.1f, 0.1f});
    // Renderer::drawQuad({0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}, this->texture);
    // Renderer::drawChar(' ', {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f});
    Renderer::drawText("Hi Sarrahhh!\t\n%", {-0.9f, 0.8f, 0.0f}, {0.1f, 0.1f}, Color::RED);
    Renderer::end();
  }

  void ExampleLayer::onUiRender(Ui& ui) {
    ui.begin({0.0f, 0.0f});
      ui.beginLayout(Ui::Layout::Type::Vertical);
        if (ui.button({1.0f, 1.0f, 0.0f}, 0)) {
          Logger::info("Button 0 clicked!!!");
        }
        if (ui.button({1.0f, 0.0f, 0.0f}, 1)) {
          Logger::info("Button 1 clicked!!!");
        }
        if (ui.button({1.0f, 0.0f, 1.0f}, 2)) {
          Logger::info("Button 2 clicked!!!");
        }
        if (ui.button({0.0f, 0.0f, 1.0f}, 3)) {
          Logger::info("Button 3 clicked!!!");
        }
      ui.endLayout();
      ui.beginLayout(Ui::Layout::Type::Vertical);
        if (ui.button({0.0f, 0.0f, 0.0f}, 4)) {
          Logger::info("Button 4 clicked!!!");
        }
        if (ui.button({1.0f, 1.0f, 1.0f}, 5)) {
          Logger::info("Button 5 clicked!!!");
        }
        if (ui.button({0.5f, 0.5f, 0.5f}, 6)) {
          Logger::info("Button 6 clicked!!!");
        }
        if (ui.button({0.0f, 0.5f, 0.0f}, 7)) {
          Logger::info("Button 7 clicked!!!");
        }
        ui.beginLayout(Ui::Layout::Type::Horizontal);
          if (ui.button({1.0f, 1.0f, 0.5f}, 8)) {
            Logger::info("Button 8 clicked!!!");
          }
          if (ui.button({1.0f, 0.5f, 0.5f}, 9)) {
            Logger::info("Button 9 clicked!!!");
          }
        ui.endLayout();
      ui.endLayout();
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
#include <string>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Manager.hpp"
#include "Layers/ExampleLayer.hpp"

#include "Application.hpp"

namespace Game {

  ExampleLayer::ExampleLayer(f32 aspectRatio)
  : mCameraController(aspectRatio),
    mTexture{ResourceManager::loadTexture("thinking-emoji.png")}
  {}

  void ExampleLayer::onAttach() {
    Logger::info("ExampleLayer::onAttach was called");
  }

  void ExampleLayer::onDetach() {
    Logger::info("ExampleLayer::onDetach was called");
  }

  void ExampleLayer::onUpdate(Timestep ts) {
    auto fps = 1.0f / ts;
    std::stringstream ss;
    ss.precision(2);
    ss << std::fixed << (1.0f / ts) << "fps";
    Renderer::begin(mCameraController.getCamera());
    Renderer::drawText("HELLO, WORLD!!!", {-0.8f, 0.0f, 0.0f}, {0.08f, 0.1f}, mColor);
    // Renderer::drawQuad({-0.8f, 0.8f, 0.0f}, {0.8f * 2, 0.8f * 2}, this->texture);
    mCameraController.offsetRotation(-28.0f * ts);
    mColor.r = glm::sin(mCameraController.getRotation());
    mColor.b = glm::cos(mCameraController.getRotation());
    // Renderer::drawCharacter(' ', {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f});
    // Renderer::drawText("Hello, world!", {-0.9f, 0.7f, 0.0f}, {0.1f, 0.1f}, Color::GREEN);
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
    event.dispatch(&ExampleLayer::onWindowResizeEvent, this);
    event.dispatch(&ExampleLayer::onMouseScrollEvent, this);
    event.dispatch(&ExampleLayer::onKeyPressedEvent, this);
  }

  bool ExampleLayer::onWindowResizeEvent(const WindowResizeEvent& event) {
    mCameraController.resize(event.getWidth(), event.getHeight());
    return false;
  }

  bool ExampleLayer::onMouseScrollEvent(const MouseScrollEvent& event) {
    mCameraController.offsetZoomLevel( -event.getYOffset() * 0.25f );
    return false;
  }

  bool ExampleLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    if (event.getKey() == Key::R) {
      ResourceManager::reloadTextures();
    }
    return true;
  }

} // namespace Game
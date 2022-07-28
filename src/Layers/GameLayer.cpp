#include <string>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Manager.hpp"
#include "Layers/GameLayer.hpp"

#include "Application.hpp"

namespace Game {

  GameLayer::GameLayer(f32 aspectRatio)
  : mCameraController(Vec3{0.0f, 0.0f, 3.0f}, 45.0f, aspectRatio),
    mTexture{ResourceManager::loadTexture("thinking-emoji.png")}
  {}

  void GameLayer::onAttach() {
    Logger::info("GameLayer::onAttach was called");
  }

  void GameLayer::onDetach() {
    Logger::info("GameLayer::onDetach was called");
  }

  void GameLayer::onUpdate(Timestep ts) {
    Renderer::begin(mCameraController.getCamera());
    Renderer::drawText("HELLO, WORLD!!!", {-0.8f, 0.0f, 0.0f}, {0.08f, 0.1f}, mColor);
    Renderer::drawQuad({0.0f, 0.0f, 0.0f}, {1, 1}, Color::RED);
    Renderer::drawQuad({0.0f, 0.0f, 1.0f}, {1, 1}, Color::GREEN);
    Renderer::drawQuad({0.0f, 0.0f, -1.0f}, {1, 1}, Color::BLUE);
    Renderer::end();
  }

  void GameLayer::onUiRender(Ui& ui) {
  }

  void GameLayer::onEvent(const Event& event) {
    Logger::error("ffffgggggggggggggggggggdddddddddddddddddddddf");
    mCameraController.onEvent(event);
    event.dispatch(&GameLayer::onWindowResizeEvent, this);
    event.dispatch(&GameLayer::onMouseScrollEvent, this);
    event.dispatch(&GameLayer::onKeyPressedEvent, this);
  }

  bool GameLayer::onWindowResizeEvent(const WindowResizeEvent& event) {
    mCameraController.resize(event.getWidth(), event.getHeight());
    return false;
  }

  bool GameLayer::onMouseScrollEvent(const MouseScrollEvent& event) {
    return false;
  }

  bool GameLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    
    return false;
  }

} // namespace Game
#include <string>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Core/Input.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Manager.hpp"
#include "Layers/GameLayer.hpp"

#include "Application.hpp"

namespace Game {

  GameLayer::GameLayer(f32 aspectRatio)
  : mCameraController(Vec3{0.0f, 0.0f, 3.0f}, 45.0f, aspectRatio),
    mTexture{ResourceManager::loadTexture("stallTexture.png")},
    mShader{ResourceManager::loadShader("Light.glsl")},
    mCubeMesh{ResourceManager::loadMesh("stall.obj")}
  {}

  void GameLayer::onAttach() {
    Logger::info("GameLayer::onAttach was called");
    Application::getWindow().enableCursor(mCaptureCursor);
  }

  void GameLayer::onDetach() {
    Logger::info("GameLayer::onDetach was called");
    Application::getWindow().enableCursor(false);
  }

  void GameLayer::onUpdate(Timestep ts) {
    mCameraController.onUpdate(ts);
    Renderer::begin(mCameraController.getCamera());
    // Renderer::drawText("HELLO, WORLD!!!", {-0.8f, 0.0f, 0.0f}, {0.08f, 0.1f}, mColor);
    // Renderer::drawQuad({0.0f, 0.0f, 0.0f}, {1, 1}, Color::RED);
    // Renderer::drawQuad({0.0f, 0.0f, 1.0f}, {1, 1}, Color::GREEN);
    // Renderer::drawQuad({0.0f, 0.0f, -1.0f}, {1, 1}, Color::BLUE);
    Renderer::draw(mShader, mCubeMesh, mTexture);
    Renderer::end();
  }

  void GameLayer::onUiRender(Ui& ui) {
  }

  void GameLayer::onEvent(const Event& event) {
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
    switch (event.getKey()) {
      case Key::C:
        mCaptureCursor = !mCaptureCursor;
        Application::getWindow().enableCursor(mCaptureCursor);
        break;
      case Key::Escape:
        Application::get().quit();
        break;
    }
    if (event.getKey() == Key::Escape) {
      Application::get().quit();
    }
    return false;
  }

} // namespace Game
#include <string>
#include <sstream>

#include <glm/glm.hpp>
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
  : mO(aspectRatio),
    mCameraController(Vec3{0.0f, 0.0f, 3.0f}, 45.0f, aspectRatio),
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
    auto fps = 1.0f / ts;
    std::stringstream ss;
    ss.precision(4);
    ss << std::fixed << (1.0f / ts) << "fps" << '\n';

    Renderer::begin(mO.getCamera());
    Renderer::drawText(ss.str(), {-0.95f, 0.95, 0.0f}, {0.1f, 0.1f}, mColor);
    Renderer::end();
    // Renderer::drawQuad({0.0f, 0.0f, 0.0f}, {1, 1}, Color::RED);
    // Renderer::drawQuad({0.0f, 0.0f, 1.0f}, {1, 1}, Color::GREEN);
    // Renderer::drawQuad({0.0f, 0.0f, -1.0f}, {1, 1}, Color::BLUE);

    mShader.bind();
    mShader.setVec3("uLightPosition", mCameraController.getPosition());
    mShader.unbind();

    Renderer::begin(mCameraController.getCamera());
    mCameraController.onUpdate(ts);
    for (u32 i = 0; i < mCount; i++) {
      Mat4 transform = Mat4(1.0f);
      transform = glm::translate(transform, Vec3{0.1f * (f32)i});
      // transform = glm::scale(transform, Vec3{f});
      Renderer::draw(mShader, mCubeMesh, mTexture, transform);
    }
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

    if (event.getKey() == Key::Up) {
      mCount++;
    }
    return false;
  }

} // namespace Game
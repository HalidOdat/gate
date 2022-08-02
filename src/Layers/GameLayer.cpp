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

  GameLayer::GameLayer()
  : mCameraController(Vec3{0.0f, 0.0f, 3.0f}, 45.0f, Application::getWindow().getAspectRatio()),
    mTextureDiffuse{ResourceManager::loadTexture("CrateDiffuse.png")},
    mTextureSpecular{ResourceManager::loadTexture("CrateSpecular.png")},
    mShader{ResourceManager::loadShader("LightMap.glsl")},
    mCubeMesh{ResourceManager::cubeMesh()}
  {}

  void GameLayer::onAttach() {
    Logger::info("GameLayer::onAttach was called");
  }

  void GameLayer::onDetach() {
    Logger::info("GameLayer::onDetach was called");
    Application::getWindow().enableCursor(false);
  }

  void GameLayer::onUpdate(Timestep ts) {
    mShader.bind();
    mShader.setVec3("uViewPosition", mCameraController.getPosition());
    if (mMoveLight) {
      mShader.setVec3("uLight.position", mCameraController.getPosition());
    }
    mShader.setVec3("uLight.ambient", mLight.ambient);
    mShader.setVec3("uLight.diffuse", mLight.diffuse);
    mShader.setVec3("uLight.specular", mLight.specular);
    
    // mShader.setVec3("uMaterial.ambient", mMaterial.ambient);
    mTextureSpecular.bind(1);

    mShader.setInt("uMaterial.diffuse", 0);
    mShader.setInt("uMaterial.specular", 1);
    mShader.setFloat("uMaterial.shininess", mMaterial.shininess);
    mShader.unbind();


    if (mCaptureCursor) {
      mCameraController.onUpdate(ts);
    }
    Renderer::begin(mCameraController.getCamera());
    Renderer::drawQuad({4, 4}, {0.5f, 0.5f}, Color::GREEN);
    Renderer::drawText("A B C D E F G H I J K L M N O P", {-4, -4}, 0.5f);
    for (u32 i = 0; i < mCount; i++) {
      Mat4 transform = Mat4(1.0f);
      transform = glm::translate(transform, Vec3{0.01f * (f32)i});
      Renderer::draw(mShader, mCubeMesh, mTextureDiffuse, transform);
    }
    Renderer::end();
  }

  void GameLayer::onUiRender(Ui& ui) {
    ui.begin({0, 200});
      ui.slider(mMaterial.shininess, 0.1f, 128.0f);
    ui.end();
  }

  void GameLayer::onEvent(const Event& event) {
    if (mCaptureCursor) {
      mCameraController.onEvent(event);
    }
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
        Application::getWindow().enableCursor(mCaptureCursor);
        mCaptureCursor = !mCaptureCursor;
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

    if (event.getKey() == Key::M) {
      mMoveLight = !mMoveLight;
    }

    switch (event.getKey()) {
      case Key::_1:
        mMaterial.ambient -= 0.1;
        break;
      case Key::_2:
        mMaterial.ambient += 0.1;
        break;
      case Key::_3:
        mMaterial.diffuse -= 0.1;
        break;
      case Key::_4:
        mMaterial.diffuse += 0.1;
        break;
      case Key::_5:
        mMaterial.specular -= 0.1;
        break;
      case Key::_6:
        mMaterial.specular += 0.1;
        break;
      case Key::_7:
        mMaterial.shininess -= 1;
        break;
      case Key::_8:
        mMaterial.shininess += 1;
        break;
    }
    return false;
  }

} // namespace Game
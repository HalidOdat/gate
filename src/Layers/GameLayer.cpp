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
    mShader{Shader::load("asserts/shaders/SpotLight.glsl").build()},
    mCubeMesh{Mesh::cube()},
    mScene{new Scene("Scene")}
  {}

  void GameLayer::onAttach() {
    Logger::info("GameLayer::onAttach was called");

    mMaterial.diffuseMap  = Texture2D::load("assets/textures/CrateDiffuse.png").build();
    mMaterial.specularMap = Texture2D::load("assets/textures/CrateSpecular.png").build();
    mMaterial.emissionMap = Texture2D::load("assets/textures/matrix.jpg").build();
    mMaterial.shininess   = 32.0f;

    static Vec3 cubePositions[] = {
      Vec3( 0.0f,  0.0f,  0.0f),
      Vec3( 2.0f,  5.0f, -15.0f),
      Vec3(-1.5f, -2.2f, -2.5f),
      Vec3(-3.8f, -2.0f, -12.3f),
      Vec3( 2.4f, -0.4f, -3.5f),
      Vec3(-1.7f,  3.0f, -7.5f),
      Vec3( 1.3f, -2.0f, -2.5f),
      Vec3( 1.5f,  2.0f, -2.5f),
      Vec3( 1.5f,  0.2f, -1.5f),
      Vec3(-1.3f,  1.0f, -1.5f)
    };

    // for (u32 i = 0; i < mCount && i < 10; i++) {
    //   Mat4 transform = Mat4(1.0f);
    //   transform = glm::translate(transform, cubePositions[i]);
    //   f32 angle = 20.0f * i;
    //   transform = glm::rotate(transform, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
    //   Renderer::submit(mShader, mCubeMesh, mMaterial, transform);
    // }

    for (u32 i = 0; i < 10; i++) {
      Entity entity = mScene->createEntity(String("box ") + std::to_string(i));
      entity.add<TransformComponent>(cubePositions[i]);
      entity.add<MeshSourceComponent>(mCubeMesh);
      // entity.add<MeshRendererComponent>(mMaterial);
      mEntity = entity;
    }
  }

  void GameLayer::onDetach() {
    Logger::info("GameLayer::onDetach was called");
    Application::getWindow().enableCursor(false);
  }

  void GameLayer::onUpdate(Timestep ts) {
    mShader->bind();
    mShader->setVec3("uViewPosition", mCameraController.getPosition());
    if (mMoveLight) {
      mShader->setVec3("uLight.position", mCameraController.getPosition());
    }
    mShader->setVec3("uLight.direction", mCameraController.getFront());
    mShader->setFloat("uLight.cutOff", glm::cos(glm::radians(12.5f)));
    mShader->setFloat("uLight.outerCutOff", glm::cos(glm::radians(17.5f)));

    mShader->setVec3("uLight.ambient", mLight.ambient);
    mShader->setVec3("uLight.diffuse", mLight.diffuse);
    mShader->setVec3("uLight.specular", mLight.specular);

    mShader->setFloat("uLight.constant",  1.0f);
    mShader->setFloat("uLight.linear",    0.09f);
    mShader->setFloat("uLight.quadratic", 0.032f);

    if (mCaptureCursor) {
      mCameraController.onUpdate(ts);
    }

    Renderer::begin3D(mCameraController);
    mScene->onUpdate(ts);
    Renderer::end();

    // Renderer::begin(mCameraController.getCamera());
    // Renderer::drawText("A B C D E F G H I J K L M N O P", {-4, -4}, 0.5f);
    // Renderer::drawQuad({4, 4}, {0.5f, 0.5f}, Color::GREEN);
    // Renderer::end();
  }

  void GameLayer::onUiRender(Ui& ui) {
    ui.begin({0, 200});
      ui.slider(mMaterial.transparency, 0.0f, 1.0f);
      ui.slider(mLight.ambient,  Vec3(0.1f), Vec3(1.0f));
      ui.slider(mLight.diffuse,  Vec3(0.1f), Vec3(1.0f));
      ui.slider(mLight.specular, Vec3(0.1f), Vec3(1.0f));
      //ui.slider(mEntity.get<TransformComponent>().rotation, Vec3(0.0f), Vec3(8.0f));
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

  bool GameLayer::onMouseScrollEvent(const MouseScrollEvent&) {
    return false;
  }

  bool GameLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    switch (event.getKey()) {
      case Key::C:
        Application::getWindow().enableCursor(mCaptureCursor);
        mCaptureCursor = !mCaptureCursor;
        mCameraController.resetLastPosition();
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

    return false;
  }

} // namespace Game
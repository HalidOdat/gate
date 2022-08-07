#include <string>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Manager.hpp"
#include "Layers/EditorLayer.hpp"

#include "Application.hpp"

#include "Scene/Scene.hpp"

namespace Game {

  EditorLayer::EditorLayer()
    : mEditorCameraController(Application::getWindow().getAspectRatio())
    , mCameraController(Vec3{0.0f, 0.0f, 3.0f}, 45.0f, Application::getWindow().getAspectRatio())
  {}

  void EditorLayer::onAttach() {
    mEditorScene.reset(new Scene("New Scene"));
    mActiveScene = mEditorScene;

    Material::Handle material(new Material());
    material->setDiffuseMap(ResourceManager::loadTexture("CrateDiffuse.png"));
    material->setSpecularMap(ResourceManager::loadTexture("CrateSpecular.png"));
    material->setEmissionMap(ResourceManager::loadTexture("matrix.jpg"));
    material->setShininess(32.0f);

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
    
    Mesh::Handle mesh = ResourceManager::cubeMesh();

    for (u32 i = 0; i < 10; i++) {
      Entity entity = mEditorScene->createEntity(String("box ") + std::to_string(i));
      entity.add<TransformComponent>(cubePositions[i % 10] + (f32)i * Vec3(0.2f), Vec3(0.1f, 0.2f, 0.3f) * (f32)i);
      entity.add<MeshSourceComponent>(mesh);
      entity.add<MeshRendererComponent>(material);
    }

    auto node = Serializer::Node(*mEditorScene);
    Logger::info("Object: %s", node.toString().c_str());

    Logger::info("EditorLayer::onAttach was called");
  }

  void EditorLayer::onDetach() {
    Logger::info("EditorLayer::onDetach was called");
  }

  void EditorLayer::onUpdate(Timestep ts) {
    mCameraController.onUpdate(ts);
    switch (mState) {
      case State::Edit:
        GAME_TODO("not implemented yet!");
        mActiveScene->render(mCameraController);
        break;
      case State::Play:
        mActiveScene->render(mCameraController);
        break;
      default:
        GAME_UNREACHABLE("Unknown state!");
    }
  }

  void EditorLayer::onUiRender(Ui& ui) {
    if (mShow) {
      auto ts = Timestep::get();
      const auto ar = Application::getWindow().getAspectRatio();
      Renderer::begin(mCameraController.getCamera());
      auto fps = 1.0f / ts;
      std::stringstream ss;
      ss.precision(2);
      ss << std::fixed << (1.0f / ts) << "fps" << '\n';

      std::string fpsString = ss.str();

      ui.begin({0.0f, 0.0f});
        if (ui.button(fpsString, 0)) {
          Logger::info("Button 0 clicked!!!");
        }
        if (ui.button("Button 1", 1)) {
          Logger::info("Button 1 clicked!!!");
        }
        if (ui.button("Button 2", 2)) {
          Logger::info("Button 2 clicked!!!");
        }
        if (ui.button("Button 3", 3)) {
          Logger::info("Button 3 clicked!!!");
        }
      ui.end();
    }
  }

  void EditorLayer::onEvent(const Event& event) {
    mCameraController.onEvent(event);

    event.dispatch(&EditorLayer::onWindowResizeEvent, this);
    event.dispatch(&EditorLayer::onMouseScrollEvent, this);
    event.dispatch(&EditorLayer::onKeyPressedEvent, this);
  }

  bool EditorLayer::onWindowResizeEvent(const WindowResizeEvent& event) {
    auto[width, height] = event.getSize();
    mEditorCameraController.resize(width, height);
    return false;
  }

  bool EditorLayer::onMouseScrollEvent(const MouseScrollEvent& event) {
    return false;
  }

  bool EditorLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    if (event.getKey() == Key::Escape) {
      Application::get().quit();
    }

    if (event.getKey() == Key::F3) {
      mShow = !mShow;
      return true;
    }

    if (mShow) {
      if (event.getKey() == Key::T) {
        ResourceManager::reloadAll<Texture2D>();
        return true;
      }
      if (event.getKey() == Key::Z) {
        ResourceManager::reloadAll<Shader>();
        return true;
      }
      if (event.getKey() == Key::M) {
        ResourceManager::reloadAll<Mesh>();
        return true;
      }
    }

    return false;
  }

} // namespace Game
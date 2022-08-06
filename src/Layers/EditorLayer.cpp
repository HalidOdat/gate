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
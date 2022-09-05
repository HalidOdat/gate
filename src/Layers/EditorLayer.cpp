#include <string>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Renderer/Renderer.hpp"
#include "Resource/Manager.hpp"
#include "Layers/EditorLayer.hpp"

#include "Application.hpp"

#include "Scene/Scene.hpp"
#include "Scene/SceneSerializer.hpp"

#include "Core/Base.hpp"
#include "Core/Input.hpp"

namespace Game {

  EditorLayer::EditorLayer()
    : mEditorCameraController(Application::getWindow().getAspectRatio())
    , mCameraController(Vec3{0.0f, 0.0f, 3.0f}, 45.0f, Application::getWindow().getAspectRatio())
  {}

  void EditorLayer::onAttach() {
    GAME_PROFILE_FUNCTION();

    mEditorScene.reset(new Scene("New Scene"));
    mActiveScene = mEditorScene;

    const auto scenePath = "assets/scenes/default.scene.json";

    if (!SceneSerializer::deserializeFromFile(scenePath, *mEditorScene)) {
      Logger::error("Editor: Couldn't deserialize scene from file");
    } else {
      Logger::info("Editor: Loaded '%s' scene file", scenePath);
    }

    Application::getWindow().setTitle(mEditorScene->getName());

    Logger::info("EditorLayer::onAttach was called");
  }

  void EditorLayer::onDetach() {
    GAME_PROFILE_FUNCTION();
    Logger::info("EditorLayer::onDetach was called");
  }

  void EditorLayer::onUpdate(Timestep ts) {
    GAME_PROFILE_FUNCTION();
    if (!Input::isKeyPressed(Key::LeftControl) && !Input::isKeyPressed(Key::RightControl)) {
      mCameraController.onUpdate(ts);
    }
    switch (mState) {
      case State::Edit:
        mActiveScene->render(mCameraController);
        break;
      case State::Simulate:
        mActiveScene->onSimulateUpdate(ts, mCameraController);
        break;
      case State::Play:
        mActiveScene->render(mCameraController);
        break;
      default:
        GAME_UNREACHABLE("Unknown state!");
    }

    if (mSelectedEntity.isValid()) {
      if (Input::isKeyPressed(Key::LeftControl) || Input::isKeyPressed(Key::RightControl)) {
        auto& transform = mSelectedEntity.get<TransformComponent>();
        if (Input::isKeyPressed(Key::Left)) {
            transform.translation.x -= ts * 20;
        }
        if (Input::isKeyPressed(Key::Right)) {
            transform.translation.x += ts * 20;
        }
        if (Input::isKeyPressed(Key::LeftShift) || Input::isKeyPressed(Key::RightShift)) {
          if (Input::isKeyPressed(Key::Up)) {
            transform.translation.z -= ts * 20;
          }
          if (Input::isKeyPressed(Key::Down)) {
            transform.translation.z += ts * 20;
          }
        } else {
          if (Input::isKeyPressed(Key::Up)) {
            transform.translation.y += ts * 20;
          }
          if (Input::isKeyPressed(Key::Down)) {
            transform.translation.y -= ts * 20;
          }
        }
      }
    }
  }

  void EditorLayer::onUiRender(Ui& ui) {
    std::stringstream ss;
    ss.precision(2);
    ss << std::fixed << (1.0f / Timestep::get()) << "fps" << '\n';
    std::string fpsString = ss.str();

    ui.beginDock(Ui::Dock::Left, 20.0f);
      if (ui.button(fpsString, 100000)) {
        Logger::info("Button 0 clicked!!!");
      }
      auto view = mEditorScene->mRegistry.view<TagComponent>();
      for (auto[entity, tag] : view) {
        if (ui.button(tag.tag, entity.getId())) {
          mSelectedEntity = Entity(entity, mEditorScene.get());
        }
      }
      if (ui.checkbox(mShow)) {}
      if (mSelectedEntity.isValid()) {
        auto& transform = mSelectedEntity.get<TransformComponent>();
        ui.slider(transform.translation, Vec3(-20.0f), Vec3(20.0f));
        ui.slider(transform.rotation, Vec3(-20.0f), Vec3(20.0f));
        ui.slider(transform.scale, Vec3(1.0f), Vec3(40.0f));
      }
    ui.end();
  }

  void EditorLayer::onEvent(const Event& event) {
    if (mClicked) {
      mCameraController.onEvent(event);
    } else {
      mCameraController.resetLastPosition();
    }

    event.dispatch(&EditorLayer::onWindowResizeEvent, this);
    event.dispatch(&EditorLayer::onKeyPressedEvent, this);
    event.dispatch(&EditorLayer::onMouseScrollEvent, this);
    event.dispatch(&EditorLayer::onMouseButtonPressedEvent, this);
    event.dispatch(&EditorLayer::onMouseButtonReleasedEvent, this);
  }

  bool EditorLayer::onWindowResizeEvent(const WindowResizeEvent& event) {
    auto[width, height] = event.getSize();
    mEditorCameraController.resize(width, height);
    return false;
  }

  bool EditorLayer::onMouseScrollEvent(const MouseScrollEvent& event) {
    (void)event;
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

    if (event.getModifier() == KeyModifier::Control) {
      if (event.getKey() == Key::S) {
        mState = State::Simulate;
        Logger::trace("Editor: Simulation has started");
        return true;
      }
    }
    
    if (mShow) {
      // if (event.getKey() == Key::T) {
      //   ResourceManager::reloadAll<Texture2D>();
      //   return true;
      // }
      // if (event.getKey() == Key::Z) {
      //   ResourceManager::reloadAll<Shader>();
      //   return true;
      // }
      // if (event.getKey() == Key::M) {
      //   ResourceManager::reloadAll<Mesh>();
      //   return true;
      // }
    }

    return false;
  }

  bool EditorLayer::onMouseButtonPressedEvent(const MouseButtonPressedEvent& event) {
    if (event.getButton() == MouseButton::Left) {
      mClicked = true;
    }
    return false;
  }
  bool EditorLayer::onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event) {
    if (event.getButton() == MouseButton::Left) {
      mClicked = false;
    }
    return false;
  }

} // namespace Game
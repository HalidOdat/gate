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

    for (u32 i = 0; i < 10; i++) {
      Entity ent{i, mEditorScene.get()};
      Vec3 position = ent.get<TransformComponent>().getTranslation();
      ent.add<CollisionComponent>(Physics::BoundingSphere(position, 4.0f));
    }

    // Entity ent{0, mEditorScene.get()};
    // for (int i = 0; i < 100; i++) {
    //   for (int j = 0; j < 100; j++) {
    //     for (int k = 0; k < 10; k++) {
    //       Entity newEnt = ent.duplicate();
    //       auto& transform = newEnt.get<TransformComponent>();
    //       transform.setTranslation({i * 2.0f, j * 2.0f, k * 2.0f});
    //     }
    //   }
    // }
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
        auto amount = ts * 20.0f;
        if (Input::isKeyPressed(Key::Left)) {
          transform.offsetTranslation({-amount, 0.0f, 0.0f});
        }
        if (Input::isKeyPressed(Key::Right)) {
          transform.offsetTranslation({amount, 0.0f, 0.0f});
        }
        if (Input::isKeyPressed(Key::LeftShift) || Input::isKeyPressed(Key::RightShift)) {
          if (Input::isKeyPressed(Key::Up)) {
            transform.offsetTranslation({0.0f, 0.0f, -amount});
          }
          if (Input::isKeyPressed(Key::Down)) {
            transform.offsetTranslation({0.0f, 0.0f, amount});
          }
        } else {
          if (Input::isKeyPressed(Key::Up)) {
            transform.offsetTranslation({0.0f, amount, 0.0f});
          }
          if (Input::isKeyPressed(Key::Down)) {
            transform.offsetTranslation({0.0f, -amount, 0.0f});
          }
        }
      }
    }

    std::stringstream ss;
    ss.precision(2);
    ss << mEditorScene->getName() << " - " << std::fixed << (1.0f / Timestep::get()) << "fps / " << Timestep::get() * 1000.0f << "ms";
    std::string fpsString = ss.str();
    Application::getWindow().setTitle(fpsString.c_str());
  }

  void EditorLayer::onUiRender(Ui& ui) {
    ui.beginDock(Ui::Dock::Left, 20.0f);
      auto view = mEditorScene->mRegistry.view<TagComponent>();
      for (auto[entity, tag] : view) {
        if (ui.button(tag.tag, entity.getId())) {
          mSelectedEntity = Entity(entity, mEditorScene.get());
        }
      }
      if (ui.checkbox(mShow)) {}
      if (mSelectedEntity.isValid()) {
        auto& transform = mSelectedEntity.get<TransformComponent>();
        auto translation = transform.getTranslation();
        auto rotation = transform.getRotation();
        auto scale = transform.getScale();
        ui.slider(translation, Vec3(-20.0f), Vec3(20.0f));
        ui.slider(rotation, Vec3(-20.0f), Vec3(20.0f));
        ui.slider(scale, Vec3(1.0f), Vec3(40.0f));
        transform.setTranslation(translation);
        transform.setRotation(rotation);
        transform.setScale(scale);
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
    event.dispatch(&EditorLayer::onMouseMoveEvent, this);
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
      switch (event.getKey()) {
        case Key::S:
          mState = State::Simulate;
          Logger::trace("Editor: Simulation has started");
          break;
        case Key::D:
          if (mSelectedEntity) {
            mSelectedEntity.duplicate();
          }
          break;
        default:
          return false;
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

      u32 entityId = Renderer::readPixel(mLastMousePosition.x, mLastMousePosition.y);
      if (entityId != UINT32_MAX) {
        Logger::trace("Editor: Selected entity id: 0x%x", entityId);
        mSelectedEntity = {entityId, mEditorScene.get()};
        Renderer::setSelectedEntity(entityId);
      }
    }
    return false;
  }
  bool EditorLayer::onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event) {
    if (event.getButton() == MouseButton::Left) {
      mClicked = false;
    }
    return false;
  }
  bool EditorLayer::onMouseMoveEvent(const MouseMoveEvent& event) {
    mLastMousePosition = {event.getX(), event.getY()};
    return false;
  }

} // namespace Game
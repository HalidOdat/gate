#pragma once

#include "Core/Math.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Resource/Texture.hpp"
#include "Renderer/CameraController.hpp"
#include "Ecs/Ecs.hpp"
#include "Scene/Scene.hpp"

namespace Game {
    
  class Scene;

  class EditorLayer {
  public:
    EditorLayer();

    void onAttach();
    void onDetach();
    void onUpdate(Timestep ts);
    void onUiRender(Ui& ui);
    void onEvent(const Event& event);
  
  private:
    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onKeyPressedEvent(const KeyPressedEvent& event);
    bool onMouseScrollEvent(const MouseScrollEvent& event);
    bool onMouseMoveEvent(const MouseMoveEvent& event);
    bool onMouseButtonPressedEvent(const MouseButtonPressedEvent& event);
    bool onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event);

  private:
    OrthographicCameraController mEditorCameraController;
    PerspectiveCameraController mCameraController;

    bool mClicked = false;
    bool mShow    = true;

    Ref<Scene> mEditorScene = nullptr;
    Ref<Scene> mActiveScene = nullptr;

    Vec2 mLastMousePosition{0.0f};
    Entity mSelectedEntity;
  };

} // namespace Game

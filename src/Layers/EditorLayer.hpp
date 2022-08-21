#pragma once

#include "Core/Math.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Resource/Texture.hpp"
#include "Renderer/CameraController.hpp"
#include "Ecs/Ecs.hpp"
#include "Layers/Layer.hpp"
#include "Scene/Scene.hpp"

namespace Game {
    
  class Scene;

  class EditorLayer : public Layer {
  public:
    EditorLayer();

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onUpdate(Timestep ts) override;
    virtual void onUiRender(Ui& ui) override;
    virtual void onEvent(const Event& event) override;
  
  private:
    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onKeyPressedEvent(const KeyPressedEvent& event);
    bool onMouseScrollEvent(const MouseScrollEvent& event);
    bool onMouseButtonPressedEvent(const MouseButtonPressedEvent& event);
    bool onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event);
  
  private:
    enum class State : u8 {
      Edit,
      Play,
    };

  private:
    OrthographicCameraController mEditorCameraController;
    PerspectiveCameraController mCameraController;
    
    bool mClicked = false;

    bool  mShow  = true;
    State mState = State::Play;

    Ref<Scene> mEditorScene = nullptr;
    Ref<Scene> mActiveScene = nullptr;

    Entity mSelectedEntity;
  };

} // namespace Game

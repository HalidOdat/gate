#pragma once

#include "Core/Math.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Resource/Texture.hpp"
#include "Renderer/CameraController.hpp"
#include "Ecs/Ecs.hpp"
#include "Layers/Layer.hpp"

namespace Game {
    
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
    bool onMouseScrollEvent(const MouseScrollEvent& event);
    bool onKeyPressedEvent(const KeyPressedEvent& event);
  
  private:
    OrthographicCameraController mCameraController;

    bool mShow = true;
  };

} // namespace Game

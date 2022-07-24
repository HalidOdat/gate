#pragma once

#include "Events/KeyEvent.hpp"
#include "Renderer/Camera.hpp"
#include "Ecs/Ecs.hpp"
#include "Layers/Layer.hpp"

namespace Game {
    
  class ExampleLayer : public Layer {
  public:
    ExampleLayer();

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onUpdate(Timestep ts) override;
    virtual void onUiRender(Ui& ui) override;
    virtual void onEvent(const Event& event) override;
  
  private:
    bool onKeyPressedEvent(const KeyPressedEvent& event);

  private:
    OrthographicCamera camera;
    Registry registry;
  };

} // namespace Game

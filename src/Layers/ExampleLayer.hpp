#pragma once

#include "Core/Math.hpp"
#include "Events/KeyEvent.hpp"
#include "Resource/Texture.hpp"
#include "Renderer/Camera.hpp"
#include "Ecs/Ecs.hpp"
#include "Layers/Layer.hpp"

namespace Game {
    
  class ExampleLayer : public Layer {
  public:
    ExampleLayer(f32 aspectRatio);

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

    Texture2D texture;
    Vec4 color = Vec4(1.0f);
  };

} // namespace Game

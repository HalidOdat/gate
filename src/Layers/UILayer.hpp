#pragma once

#include "Layers/Layer.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/KeyEvent.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Camera.hpp"

namespace Game {
    
  class UILayer : public Layer {
  public:
    UILayer();

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onUpdate() override;
    virtual void onEvent(const Event& event) override;
  
  private:
    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onKeyPressedEvent(const KeyPressedEvent& event);

  private:
    OrthographicCamera camera;

    Ref<VertexArray> square;
    Ref<Shader> textureShader;
    Ref<Texture2D> texture;
  };

} // namespace Game

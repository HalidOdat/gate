#pragma once

#include <glm/mat4x4.hpp>

#include "Layers/Layer.hpp"

#include "Renderer/VertexArray.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"

#include "Events/KeyEvent.hpp"

namespace Game {
    
  class ExampleLayer : public Layer {
  public:
    ExampleLayer();

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onUpdate() override;
    virtual void onEvent(const Event& event) override;
  
  private:
    bool onKeyPressedEvent(const KeyPressedEvent& event);

  private:
    Ref<VertexArray> square;
    Ref<Shader> textureShader;
    Ref<Texture2D> texture;

    Mat4 transform = Mat4(1.0f);
  };

} // namespace Game

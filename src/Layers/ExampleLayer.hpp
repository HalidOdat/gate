#pragma once

#include "Layers/Layer.hpp"

#include "Renderer/VertexArray.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"

namespace Game {
    
  class ExampleLayer : public Layer {
  public:
    ExampleLayer();

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onUpdate() override;
    virtual void onEvent(const Event& event) override;
  
  private:
    Ref<VertexArray> square;
    Ref<Shader> flatColorShader;
    Ref<Shader> textureShader;
    Ref<Texture2D> texture;
  };

} // namespace Game

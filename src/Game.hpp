#pragma once

// Core
#include "Core/Type.hpp"
#include "Core/Log.hpp"

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

#include "Window.hpp"

#include "Application.hpp"

namespace Game {
  
  class GameApplication : public Application {
  public:
  GameApplication() {}

    inline virtual void onCreate() override {
      Logger::info("onCreate was called");

      float positions[] = {
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, // top left 
      };

      u32 indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
      };

      this->square = VertexArray::Create();
      auto vertexBuffer = VertexBuffer::Create(positions);
      vertexBuffer->setLayout({
        { BufferElement::Type::Float3 },
        { BufferElement::Type::Float3 },
        { BufferElement::Type::Float2 },
      });
      this->square->addVertexBuffer(vertexBuffer);
      auto indexBuffer = IndexBuffer::Create(indices);
      this->square->setIndexBuffer(indexBuffer);
      this->square->unbind();
    
      // this->flatColorShader = Shader::Create("./res/shaders/FlatColor.vs", "./res/shaders/FlatColor.fs");
      // this->flatColorShader->setVec4("uColor", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
      this->textureShader = Shader::Create("./res/shaders/Texture.vs", "./res/shaders/Texture.fs");
      this->textureShader->bind();
      this->textureShader->setInt("uTexture", 0);
      this->texture = Texture2D::create("res/textures/thinking-emoji.png");
      this->texture->bind();
    }

    inline virtual void onUpdate() override {
      this->textureShader->bind();
      this->square->draw();
    }

    inline virtual void onDestroy() override {
      Logger::info("onDestroy was called");
    }
    
  private:
    Ref<VertexArray> square;
    Ref<Shader> flatColorShader;
    Ref<Shader> textureShader;
    Ref<Texture2D> texture;
  };

} // namespace Game

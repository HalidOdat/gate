#include "Core/Log.hpp"

#include "Layers/ExampleLayer.hpp"

namespace Game {

  ExampleLayer::ExampleLayer() {
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
    
    this->textureShader = Shader::Create("./res/shaders/Texture.vs", "./res/shaders/Texture.fs");
    this->textureShader->bind();
    this->textureShader->setInt("uTexture", 0);
    this->texture = Texture2D::create("res/textures/thinking-emoji.png");
    this->texture->bind();
  }

  void ExampleLayer::onAttach() {
    Logger::info("onAttach was called");
  }

  void ExampleLayer::onDetach() {
    Logger::info("onDetach was called");
  }

  void ExampleLayer::onUpdate() {
    this->textureShader->bind();
    this->square->draw();
  }

  void ExampleLayer::onEvent(const Event& event) {
    (void)event;
  }


} // namespace Game
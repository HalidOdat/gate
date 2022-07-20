#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>


#include "Core/Log.hpp"
#include "Layers/ExampleLayer.hpp"

namespace Game {

  ExampleLayer::ExampleLayer() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    glEnable(GL_DEPTH_TEST);

    const float positions[] = {
       0.5f,  0.5f, -0.1f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
       0.5f, -0.5f, -0.1f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, -0.1f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // bottom left
      -0.5f,  0.5f, -0.1f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, // top left 
    };

    const u32 indices[] = {
      0, 1, 3,   // first triangle
      1, 2, 3    // second triangle
    };

    this->square = VertexArray::create();
    auto vertexBuffer = VertexBuffer::create(positions);
    vertexBuffer->setLayout({
      { BufferElement::Type::Float3 },
      { BufferElement::Type::Float3 },
      { BufferElement::Type::Float2 },
    });

    this->square->addVertexBuffer(vertexBuffer);
    auto indexBuffer = IndexBuffer::create(indices);
    this->square->setIndexBuffer(indexBuffer);
    this->square->unbind();
    
    this->textureShader = Shader::create("./res/shaders/Texture.vs", "./res/shaders/Texture.fs");
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
    this->texture->bind();
    this->textureShader->bind();
    this->square->drawIndices(6);
  }

  void ExampleLayer::onEvent(const Event& event) {
    event.dispatch(&ExampleLayer::onKeyPressedEvent, this);
  }

  bool ExampleLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    if (event.getKey() == Key::R) {
      this->transform = glm::rotate(this->transform, glm::radians(15.0f), Vec3(0.0f, 0.0f, 1.0f));

      this->textureShader->bind();
      this->textureShader->setMat4("uTransform", this->transform);
    }
    return false;
  }

} // namespace Game
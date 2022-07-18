#include <glad/glad.h>


#include "Core/Log.hpp"
#include "Layers/UILayer.hpp"

namespace Game {

  UILayer::UILayer()
    : camera(-1.0f, 1.0f, -1.0f, 1.0f)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

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
    this->texture = Texture2D::create("res/textures/alphabet-atlas.png");
    this->texture->bind();
  }

  void UILayer::onAttach() {
    Logger::info("onAttach was called");
  }

  void UILayer::onDetach() {
    Logger::info("onDetach was called");
  }

  void UILayer::onUpdate() {
    this->texture->bind();
    this->textureShader->bind();
    this->square->draw();
  }

  void UILayer::onEvent(const Event& event) {
    event.dispatch(&UILayer::onWindowResizeEvent, this);
    event.dispatch(&UILayer::onKeyPressedEvent, this);
  }

  bool UILayer::onWindowResizeEvent(const WindowResizeEvent& event) {
    // TODO: resize camera
    return false;
  }

  bool UILayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    if (event.getKey() == Key::Up) {
      this->camera.offset(glm::vec3(0.001f, 0.0f, 0.0f));

      this->textureShader->bind();
      this->textureShader->setMat4("uProjectionView", this->camera.getProjectionView());
    }
    return false;
  }

} // namespace Game
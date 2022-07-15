#include <iostream>
#include <filesystem>

#include "Game.hpp"
#include "Core/Assert.hpp"

class ExampleApplication : public Game::Application {
public:
  ExampleApplication() {}

  virtual void onCreate() override {
    Game::Logger::info("onCreate was called");

    float positions[] = {
       0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
       0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // bottom left
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, // top left 
    };
    Game::u32 indices[] = {
      0, 1, 3,   // first triangle
      1, 2, 3    // second triangle
    };

    this->square = Game::VertexArray::Create();
    auto vertexBuffer = Game::VertexBuffer::Create(positions);
    vertexBuffer->setLayout({
      { Game::BufferElement::Type::Float3 },
      { Game::BufferElement::Type::Float3 },
      { Game::BufferElement::Type::Float2 },
    });
    this->square->addVertexBuffer(vertexBuffer);
    auto indexBuffer = Game::IndexBuffer::Create(indices);
    this->square->setIndexBuffer(indexBuffer);
    this->square->unbind();
    
    // this->flatColorShader = Game::Shader::Create("./res/shaders/FlatColor.vs", "./res/shaders/FlatColor.fs");
    // this->flatColorShader->setVec4("uColor", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    this->textureShader = Game::Shader::Create("./res/shaders/Texture.vs", "./res/shaders/Texture.fs");
    this->textureShader->bind();
    this->textureShader->setInt("uTexture", 0);
    this->texture = Game::Texture2D::create("res/textures/thinking-emoji.png");
    this->texture->bind();
  }

  virtual void onUpdate() override {
    this->textureShader->bind();
    this->square->draw();
  }

  virtual void onDestroy() override {
    Game::Logger::info("onDestroy was called");
  }
private:
  Game::Ref<Game::VertexArray> square;
  Game::Ref<Game::Shader> flatColorShader;
  Game::Ref<Game::Shader> textureShader;
  Game::Ref<Game::Texture2D> texture;
};

int main() {
  ExampleApplication().start();
}
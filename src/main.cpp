#include <iostream>

#include "Game.hpp"
#include "Core/Assert.hpp"

class ExampleApplication : public Game::Application {
public:
  ExampleApplication() {}

  virtual void onCreate() override {
    Game::Logger::info("onCreate was called");

    float positions[] = {
      -0.5f, -0.5f,
      0.5f,  -0.5f,
      0.0f,   0.5f,
    };

    this->triangle = Game::VertexArray::Create();
    auto buffer = Game::VertexBuffer::Create(positions);
    buffer->setLayout({
      { Game::BufferElement::Type::Float2 }
    });
    this->triangle->addVertexBuffer(buffer);
  }

  virtual void onUpdate() override {
    this->triangle->draw();
  }

  virtual void onDestroy() override {
    Game::Logger::info("onDestroy was called");
  }
private:
  Game::Ref<Game::VertexArray> triangle;
};

int main() {
  ExampleApplication().start();
}
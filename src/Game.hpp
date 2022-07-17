#pragma once

#include "Layers/ExampleLayer.hpp"

#include "Application.hpp"

namespace Game {
  
  class GameApplication : public Application {
  public:
    GameApplication() {
      this->pushLayer(new ExampleLayer());
    }
  };

} // namespace Game

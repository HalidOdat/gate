#pragma once

#include "Layers/UILayer.hpp"
#include "Layers/ExampleLayer.hpp"

#include "Application.hpp"

namespace Game {
  
  class GameApplication : public Application {
  public:
    GameApplication() {
      // this->pushLayer(new ExampleLayer());
      this->pushLayer(new UILayer());
    }
  };

} // namespace Game

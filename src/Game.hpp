#pragma once

#include "Layers/ExampleLayer.hpp"

#include "Application.hpp"

namespace Game {
  
  class GameApplication : public Application {
  public:
    GameApplication() {
      f32 aspectRatio = Application::getWindow().getAspectRatio();
      this->pushLayer(new ExampleLayer(aspectRatio));
    }
  };

} // namespace Game
